from pxr import Tf
from pxr.Usdviewq.plugin import PluginContainer

import DatasmithUsd

# Create an instance of the DatasmithUsdPlugin class
pluginInstance = DatasmithUsd.DatasmithUsd()

def enableDirectLink(usdviewApi):
    pluginInstance.enableDirectLink()

    
def disableDirectLink(usdviewApi):    
    pluginInstance.disableDirectLink()

def addStage(usdviewApi):
    usdStage = usdviewApi.stage
    pluginInstance.addStage(usdStage)

class DatasmithUsdPluginContainer(PluginContainer):
    def registerPlugins(self, plugRegistry, usdviewApi):            
        self._enableDirectLink = plugRegistry.registerCommandPlugin(
            "DatasmithUsdPluginContainer.enableDirectLink",
            "Enable Direct Link",
            enableDirectLink)
        self._disableDirectLink = plugRegistry.registerCommandPlugin(
            "DatasmithUsdPluginContainer.disableDirectLink",
            "Disable Direct Link",
            disableDirectLink)
        self._addStage = plugRegistry.registerCommandPlugin(
            "DatasmithUsdPluginContainer.addStage",
            "Add Stage",
            addStage)

    def configureView(self, plugRegistry, plugUIBuilder):
        tutMenu = plugUIBuilder.findOrCreateMenu("Datasmith")
        tutMenu.addItem(self._enableDirectLink)
        tutMenu.addItem(self._disableDirectLink)
        tutMenu.addItem(self._addStage)


Tf.Type.Define(DatasmithUsdPluginContainer)