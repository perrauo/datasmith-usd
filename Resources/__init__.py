from pxr import Tf
from pxr.Usdviewq.plugin import PluginContainer

import DatasmithUsd

# Create an instance of the DatasmithUsdPlugin class
plugin_instance = DatasmithUsd.DatasmithUsd()

def export(usdviewApi):
    # Call the export method
    plugin_instance.export()

class DatasmithUsdPluginContainer(PluginContainer):
    def registerPlugins(self, plugRegistry, usdviewApi):            
        self._export = plugRegistry.registerCommandPlugin(
            "DatasmithUsdPluginContainer.export",
            "Datasmith Export",
            export)

    def configureView(self, plugRegistry, plugUIBuilder):
        tutMenu = plugUIBuilder.findOrCreateMenu("Datasmith")
        tutMenu.addItem(self._export)

Tf.Type.Define(DatasmithUsdPluginContainer)