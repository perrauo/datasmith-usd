# Datasmith USD Plugin

This project is a plugin for integrating Datasmith with USD (Universal Scene Description) using DirectLink. It allows for the translation of USD meshes and the synchronization of scenes between Datasmith and USD.

## Features

- **DirectLink Integration**: Enables real-time synchronization of scenes between Datasmith and USD.
- **USD Mesh Translation**: Converts USD meshes to Datasmith meshes for further processing and export.
- **Endpoint Observer**: Monitors the connection status of DirectLink endpoints.

## Installation

1. Clone the repository.
2. Ensure you have the required dependencies installed:
   - Unreal Engine
   - USD SDK
   - Boost
   - Python 3.9
3. Update the library paths in the build configuration files to match your system setup.

## Usage

### Enabling DirectLink

To enable DirectLink, use the `EnableDirectLink` method of the `FModule` class. This will initialize the DirectLink connection and set up the necessary endpoints.

### Adding a USD Stage

To add a USD stage, use the `AddStage` method of the `FModule` class. This will load the USD stage and translate the meshes to Datasmith meshes.

### Synchronizing the Scene

To synchronize the scene, use the `Sync` method of the `FModule` class. This will update the Datasmith scene with the latest changes from the USD stage.

## Known Issues

- **Hardcoded Library Paths**: The current build configuration contains hardcoded library paths. Make sure to update these paths to match your system setup.
- **USDView Limitations**: USDView is not meant for editing data, thus the Python plugin provided in this project is rather useless. However, the same mesh translation and DirectLink can be used with any more competent USD data editor such as Maya USD.

## License

This project is licensed under the MIT License.

## Acknowledgements

- Epic Games for providing the Datasmith SDK.
- Pixar for developing USD.

