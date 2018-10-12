#pragma once

// Number of samples needs to be the same at image creation,
// renderpass creation and pipeline creation.
#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT

// Number of viewports and number of scissors have to be the same
// at pipeline creation and in any call to set them dynamically
// They also have to be the same as each other
#define NUM_VIEWPORTS 1
#define NUM_SCISSORS NUM_VIEWPORTS
