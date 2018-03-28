Miscellaneous small code snippets.

## [PrintStream](../../tree/master/PrintStream)

A hierarchy of classes that represent abstract concept of a text-based stream that can be printed into, using methods like `print(const char* str)`, `printf(const char* format, ....)` etc. Derived classes offer printing to console (standard output), to file, to memory buffer and more.

## [DisplaySettingsTest](../../tree/master/DisplaySettingsTest)

A simple Windows console C program that demonstrates how to enumerate and change display modes (screen resolution and refresh rate). See my blog post: [How to change display mode using WinAPI?](http://asawicki.info/news_1637_how_to_change_display_mode_using_winapi.html).

## [IsLog10.c](IsLog10.c)

Simple C program demonstrating multiple solutions to a question: "Write a function that checks whether an integer number is a power of 10." Contains a set of tests. See my blog post: [How to check if an integer number is a power of 10?](http://www.asawicki.info/news_1660_how_to_check_if_an_integer_number_is_a_power_of_10.html).

## [QueryPerformanceCounterTest.cpp](QueryPerformanceCounterTest.cpp)

Simple C++ console program that tests how long it takes to call WinAPI function `QueryPerformanceCounter`. See my blog post: [When QueryPerformanceCounter call takes long time](http://asawicki.info/news_1667_when_queryperformancecounter_call_takes_long_time.html).

## [VulkanAfterCrash.h](VulkanAfterCrash.h)

Simple, single-header, C++ library for Vulkan that simplifies writing 32-bit markers to a buffer that can be read after graphics driver crash and thus help you find out which specific draw call or other command caused the crash, pretty much like [NVIDIA Aftermath](https://developer.nvidia.com/nvidia-aftermath) library for Direct3D 11/12. See my blog post: [Debugging Vulkan driver crash - equivalent of NVIDIA Aftermath](http://asawicki.info/news_1677_debugging_vulkan_driver_crash_-_equivalent_of_nvidia_aftermath.html).
