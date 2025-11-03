#include "led.h"
#include "button.h"
#include "timer.h"

#define WS2812_PIN 25
#define LED_TASK_STACK_SIZE 2048
#define LED_TASK_PRIORITY 2
#define LED_CORE 0 // Run on core 0 since core 1 is used by Arduino loop

TaskHandle_t ledTaskHandle = NULL;
static NeoPixelBus<NeoRgbFeature, NeoWs2812Method> strip(1, WS2812_PIN);
static NeoPixelAnimator animations(2);
static volatile LedMode currentMode = LedMode::Off;
static volatile LedMode lastMode;

static unsigned long lastFlashTime = 0;
static boolean flashState = false;

static bool isEncoderTriggeredFlash = false; // Track which input triggered the flash

struct MyAnimationState
{
    RgbColor StartingColor;
    RgbColor EndingColor;
};

static MyAnimationState animationState[1];

static void BlendAnimUpdate(const AnimationParam &param)
{
    RgbColor updatedColor = RgbColor::LinearBlend(
        animationState[param.index].StartingColor,
        animationState[param.index].EndingColor,
        param.progress);

    strip.SetPixelColor(0, updatedColor);
}

static void startConfirmationFadeIn()
{
    animationState[0].StartingColor = strip.GetPixelColor<RgbColor>(0);
    animationState[0].EndingColor = RgbColor(4, 0, 153);
    animations.StartAnimation(0, 1000, BlendAnimUpdate); // 1 second fade in
}

static void startConfirmationFadeOut()
{
    animationState[0].StartingColor = strip.GetPixelColor<RgbColor>(0);
    animationState[0].EndingColor = RgbColor(0);
    animations.StartAnimation(0, 1000, BlendAnimUpdate); // 1 second fade out
}

static void handleConfirmationFlash()
{
    unsigned long now = millis();
    if (!animations.IsAnimating() && (now - lastFlashTime >= 2000))
    { // 2 second interval
        flashState = !flashState;
        lastFlashTime = now;

        if (flashState)
        {
            startConfirmationFadeIn();
        }
        else
        {
            startConfirmationFadeOut();
        }
    }

    if (animations.IsAnimating())
    {
        animations.UpdateAnimations();
        strip.Show();
    }
}

uint8_t splashscreenColorIndex = 0;

static void handleQuickAcknowledgementFlash()
{
    unsigned long now = millis();
    if (!animations.IsAnimating())
    {
        if (!flashState)
        {
            // Start the flash
            flashState = true;
            lastFlashTime = now;

            // Use different colors based on input source
            if (isEncoderTriggeredFlash)
            {
                animationState[0].StartingColor = RgbColor(179, 120, 20);
            }
            else
            {
                animationState[0].StartingColor = RgbColor(53, 105, 19);
            }
            animationState[0].EndingColor = strip.GetPixelColor<RgbColor>(0);

            animations.StartAnimation(0, isEncoderTriggeredFlash ? 100 : 300, BlendAnimUpdate);
        }
        else
        {
            // Flash animation completed, reset everything
            flashState = false;

            setLedMode(lastMode);
            return;
        }
    }

    // Update animation if it's still running
    if (animations.IsAnimating())
    {
        animations.UpdateAnimations();
        strip.Show();
    }
}

const RgbColor splashscreenColors[] = {
    RgbColor(0),
    RgbColor(48, 48, 48),
    RgbColor(32, 32, 32),
    RgbColor(0)};

static void handleSplashscreen()
{
    if (!animations.IsAnimating())
    {
        switch (splashscreenColorIndex)
        {
        case 0:
            animationState[0].StartingColor = splashscreenColors[0];
            animationState[0].EndingColor = splashscreenColors[1];
            animations.StartAnimation(0, 6000, BlendAnimUpdate);
            break;

        case 1:
            animationState[0].StartingColor = splashscreenColors[1];
            animationState[0].EndingColor = splashscreenColors[2];
            animations.StartAnimation(0, 4000, BlendAnimUpdate);
            break;

        case 2:
            animationState[0].StartingColor = splashscreenColors[2];
            animationState[0].EndingColor = splashscreenColors[3];
            animations.StartAnimation(0, 6000, BlendAnimUpdate);
            break;

        case 3:
            animationState[0].StartingColor = strip.GetPixelColor<RgbColor>(0);
            animationState[0].EndingColor = RgbColor(0);
            animations.StartAnimation(0, 4000, BlendAnimUpdate);
            break;
        }

        splashscreenColorIndex++;
        if (splashscreenColorIndex > 3)
        {
            splashscreenColorIndex = 0;
        }
    }

    if (animations.IsAnimating())
    {
        animations.UpdateAnimations();
        strip.Show();
    }
}

// track last time where we showed a quick acknowledgement flash
// so that we dont trigger on the same button press multiple times
static unsigned long lastQuickAcknowledgeFlashTime = 0;

int lastEncoderCount = 0;
volatile int *encoderCount;

void ledSetupEncoder(volatile int *encoder)
{
    encoderCount = encoder;
    lastEncoderCount = *encoder;
}

static void ledTask(void *parameter)
{
    while (1)
    {
        // Show quick acknowledgement flash if button was pressed or the encoder was turned
        if (
            (currentMode != LedMode::QuickAcknowledgementFlash) &&
            (currentMode != LedMode::TimerPaused) &&
            lastEncoderCount != *encoderCount)
        {
            Serial.println("=== Led: Quick acknowledgement flash (encoder)");
            isEncoderTriggeredFlash = true;
            setLedMode(LedMode::QuickAcknowledgementFlash);
            lastQuickAcknowledgeFlashTime = Button::instance->lastPressTime;
        }
        else if (Button::instance &&
                 (currentMode != LedMode::QuickAcknowledgementFlash) &&
                 (currentMode != LedMode::TimerPaused) &&
                 (Button::instance->lastPressTime > lastQuickAcknowledgeFlashTime)) // use time in case we miss a press due to the task
        {
            Serial.println("=== Led: Quick acknowledgement flash");
            isEncoderTriggeredFlash = false;
            setLedMode(LedMode::QuickAcknowledgementFlash);
            lastQuickAcknowledgeFlashTime = Button::instance->lastPressTime;
        }

        lastEncoderCount = *encoderCount;

        switch (currentMode)
        {
        case LedMode::Off:
            // if (strip.GetPixelColor(0) != RgbColor(0) && !animations.IsAnimating())
            // {
            //     animationState[0].StartingColor = strip.GetPixelColor<RgbColor>(0);
            //     animationState[0].EndingColor = RgbColor(0);
            //     animations.StartAnimation(0, 300, BlendAnimUpdate);
            // }

            // if (animations.IsAnimating())
            // {
            //     animations.UpdateAnimations();
            //     strip.Show();
            // }

            if (strip.GetPixelColor(0) != RgbColor(0))
            {
                strip.SetPixelColor(0, RgbColor(0));
                strip.Show();
                lastMode = LedMode::Off;
            }
            break;

        case LedMode::QuickAcknowledgementFlash:
            handleQuickAcknowledgementFlash();
            break;

        case LedMode::Splashscreen:
            handleSplashscreen();
            break;

        case LedMode::ConfirmationFlash:
            handleConfirmationFlash();
            break;

        case LedMode::TimerPaused:
            if (strip.GetPixelColor(0) != RgbColor(38 / 2, 99 / 2, 143 / 2))
            {
                strip.SetPixelColor(0, RgbColor(38 / 2, 99 / 2, 143 / 2)); // Dim blue
                strip.Show();
            }
        }
        // Small delay to prevent task from hogging CPU
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void setLedMode(LedMode mode)
{
    Serial.printf("=== Led: Setting mode from %d to %d\n", currentMode, mode);
    if (currentMode != LedMode::QuickAcknowledgementFlash)
    {
        Serial.printf("=== Led: setting last mode to %d\n", currentMode);
        lastMode = currentMode;
    }

    currentMode = mode;
    // Reset animation state when mode changes
    // ignore Animations in non-zero index
    animations.StopAll();

    // strip.SetPixelColor(0, RgbColor(0));
    // strip.Show();
    lastFlashTime = 0;
    flashState = false;
}

void setupLed()
{
    strip.Begin();
    strip.SetPixelColor(0, RgbColor(0));
    strip.Show();

    startLedTask();
}

void stopLed()
{
    if (ledTaskHandle != NULL)
    {
        vTaskDelete(ledTaskHandle);
        ledTaskHandle = NULL;
    }
}

void startLedTask()
{
    xTaskCreatePinnedToCore(
        ledTask,             // Task function
        "LED Task",          // Name
        LED_TASK_STACK_SIZE, // Stack size
        NULL,                // Parameters
        LED_TASK_PRIORITY,   // Priority
        &ledTaskHandle,      // Task handle
        LED_CORE             // Core ID
    );
}