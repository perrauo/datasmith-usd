from pxr import Tf
from pxr.Usdviewq.plugin import PluginContainer
from pxr.Usdviewq.qt import QtWidgets, QtCore

import DatasmithUsd

# Create an instance of the DatasmithUsdPlugin class
module = DatasmithUsd.Module()
globalUnicastField = QtWidgets.QLineEdit()
globalUnicastField.setText("0.0.0.0:0")
globalMulticastField = QtWidgets.QLineEdit()
globalMulticastField.setText("230.0.0.1:6666")

def enableDirectLink(usdviewApi):
    context = DatasmithUsd.ModuleContext()
    context.unicastEndpoint = globalUnicastField.text()
    context.multicastEndpoint = globalMulticastField.text()
    context.stage = usdviewApi.stage
    module.enableDirectLink(context)

    
def disableDirectLink(usdviewApi):    
    module.disableDirectLink()

def addStage(usdviewApi):
    context = DatasmithUsd.ModuleContext()
    context.unicastEndpoint = globalUnicastField.text()
    context.multicastEndpoint = globalMulticastField.text()
    context.stage = usdviewApi.stage
    module.addStage(context)

def sync(usdviewApi):
    context = DatasmithUsd.ModuleContext()
    context.unicastEndpoint = globalUnicastField.text()
    context.multicastEndpoint = globalMulticastField.text()
    module.sync(context)

class DatasmithUsdPluginContainer(PluginContainer):
    def registerPlugins(self, plugRegistry, usdviewApi):
        self._syncButton = QtWidgets.QPushButton("Sync")
        self._syncButton.clicked.connect(lambda: sync(usdviewApi))
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
        menu = plugUIBuilder.findOrCreateMenu("Datasmith")

        # Create labels
        unicastLabel = QtWidgets.QLabel("Unicast:")
        multicastLabel = QtWidgets.QLabel("Multicast:")

        # Create layouts for labels and text fields
        unicastLayout = QtWidgets.QHBoxLayout()
        unicastLayout.addWidget(unicastLabel)
        unicastLayout.addWidget(globalUnicastField)

        multicastLayout = QtWidgets.QHBoxLayout()
        multicastLayout.addWidget(multicastLabel)
        multicastLayout.addWidget(globalMulticastField)

        # Create widgets to hold the layouts
        unicastWidget = QtWidgets.QWidget()
        unicastWidget.setLayout(unicastLayout)

        multicastWidget = QtWidgets.QWidget()
        multicastWidget.setLayout(multicastLayout)

        # Add widgets to the menu
        unicastFieldAction = QtWidgets.QWidgetAction(menu._qMenu)
        unicastFieldAction.setDefaultWidget(unicastWidget)
        menu._qMenu.addAction(unicastFieldAction)

        multicastFieldAction = QtWidgets.QWidgetAction(menu._qMenu)
        multicastFieldAction.setDefaultWidget(multicastWidget)
        menu._qMenu.addAction(multicastFieldAction)

        menu.addItem(self._enableDirectLink)
        menu.addItem(self._disableDirectLink)
        menu.addItem(self._addStage)

        pushButtonAction = QtWidgets.QWidgetAction(menu._qMenu)
        pushButtonAction.setDefaultWidget(self._syncButton)
        menu._qMenu.addAction(pushButtonAction)


Tf.Type.Define(DatasmithUsdPluginContainer)