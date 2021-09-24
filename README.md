# Intel® Graphics Performance Analyzers plugin for Unreal Engine* 4

> To the [overview and setup guide](https://software.intel.com/content/www/us/en/develop/articles/unreal-engine-4-intel-gpa-usage-guide.html).

**Note:**	 No recompilation should be needed when integrating this plugin with release builds of Unreal Engine 4.26 and Unreal Engine 4.27. <br>
However, if UE4 reports an error that a module is missing or the plugin was built with a different engine version, please follow these steps:
1. Open the terminal and go to _\<UE4 install directory\>\Engine\Build\BatchFiles_
2. Run the command line: _RunUAT.bat BuildPlugin -plugin="\<Full path to downloaded plugin location\>\GPAPlugin\GPAPlugin.uplugin" -package="\<Full path to rebuilt plugin location\>\GPAPlugin"_
3. Copy the newly built GPAPlugin folder to _\<UE4 install directory\>\Plugins\Developer_

## Introduction

Intel® Graphics Performance Analyzers (Intel® GPA) is a toolset that helps to find rendering performance bottlenecks. A game or other application can be invoked via Intel GPA, allowing you to see FPS, shader details, texture details, and draw call details. Now, with the the Unreal Engine* 4 (UE4) plugin, you can capture a multi-frame stream directly while you are in Unreal Editor and analyze it in Graphics Frame Analyzer.

### Key Goals

* **Full integration of Intel GPA with the UE4 Editor UI**
* **Single click to capture a multi-frame stream and start performance analysis**
* **Integrated into UE4 console to capture streams from a running game**

![Alt text](Images/UI_integration.png?raw=true "Unreal Engine 4 GPA plugin integration")

## Requirements

- Windows 10 with DirectX 12 support is required.
- DirectX 12 capable hardware and drivers. For instance, Intel HD Graphics 4400 or newer.

For more information on Intel graphics and game code, please visit https://software.intel.com/gamedev

## Contributing

Please see
[CONTRIBUTING](TBD)
for information on how to request features, report issues, or contribute code
changes.

## License

Sample provided under MIT license, please see [LICENSE](LICENSE)

## Links
*	[Unreal Engine 4](https://www.unrealengine.com/en-US/download) - Unreal Engine 4 download page
*	[Intel Graphics Performance Analyzers](https://software.intel.com/content/www/us/en/develop/tools/graphics-performance-analyzers.html) - Intel GPA tools page.
