# Datasmith USD Plugin
THIS PLUGIN IS EXPERIMENTAL AND NOT WORKING IN ITS CURRENT STATE.
This project is a plugin for integrating Datasmith with USD (Universal Scene Description) using DirectLink. It allows for the translation of USD meshes and the synchronization of scenes between Datasmith and USD.

## Features

- **DirectLink Integration**: Enables real-time synchronization of scenes between Datasmith and USD.
- **USD Mesh Translation**: Converts USD meshes to Datasmith meshes for further processing and export.

## Installation
1. Build Unreal Engine from source (5.4.2)
3. Clone the repository in your Unreal Engine Datasmith directory
   - e.g. `C:\repos\UnrealEngine\Engine\Source\Programs\Enterprise\Datasmith`
4. Ensure you have the required software and dependencies installed:
   - Twinmotion 2024
   - Pixar USD v24.05
   - Boost 1.78
   - Python 3.9
4. Update the library paths in the build configuration files to match your system setup.
5. Run `GenerateProjectFiles.bat`
7. Build the newly generated `DatasmithUsd`target
8. Add PluginPath to environment's `PYTHONPATH` in order for it to be discovered by `UsdView`
   - `UsdView` will locate `plugInfo.json` in specified python path
   - e.g. `C:\repos\UnrealEngine\Engine\Binaries\Win64\DatasmithUsdPlugin`

## Usage

### 1. Start USD view
Start USD view with the following command and specifying the data you would like to open
`usdview usd_data\Kitchen_set\Kitchen_set.usd`
![image](https://github.com/perrauo/datasmith-usd/assets/24371247/c0947cf8-bc78-4b46-8625-c415adb6df56)

### 2. Start Twinmotion by specifying additional command line arguments
Additional command line arguments can be specified using the Unreal launcher
![image](https://github.com/perrauo/datasmith-usd/assets/24371247/381c5bb2-c635-4e84-9946-4697f949912f)
`-OpenProject=prompt -DirectLink.SourceName=Kitchen_set`

### 3. Enabling DirectLink
To enable DirectLink, use the `EnableDirectLink` method of the `FModule` class. This will initialize the DirectLink connection and set up the necessary endpoints.
![image](https://github.com/perrauo/datasmith-usd/assets/24371247/feacd806-bd70-42ee-b209-666daac007fa)
![image](https://github.com/perrauo/datasmith-usd/assets/24371247/ce910db3-4391-4ee5-b808-58c386338b2a)

### 4. Import the Direct Link Connection
![image](https://github.com/perrauo/datasmith-usd/assets/24371247/1ed2653d-649f-4441-b866-3c1c94791f5d)

### 5. Adding a USD Stage
To add a USD stage, use the `AddStage` method of the `FModule` class. This will load the USD stage and translate the meshes to Datasmith meshes.

## Known Issues

- **Hardcoded Library Paths**: The current build configuration contains hardcoded library paths. Make sure to update these paths to match your system setup.
- **Missing datasmith elements**: I have found that despite my best effort, the majority of the datasmith elements are not being transmitted correctly. Further investigation necessary.
- ![image](https://github.com/perrauo/datasmith-usd/assets/24371247/8a355349-a4dc-47de-9e81-6a86a249d38c)
- **Synchronizing the Scene** To synchronize the scene, use the `Sync` method of the `FModule` class. This is intended to update the Datasmith scene with the latest changes from the USD stage.

- **USDView Limitations**: USDView is not meant for editing data, thus the Python plugin provided in this project is rather useless. However, the same mesh translation and DirectLink can be used with any more competent USD data editor such as Maya USD.

## Acknowledgements

- Epic Games for providing the Datasmith SDK.
- Pixar for developing USD.

