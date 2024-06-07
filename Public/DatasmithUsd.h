// DatasmithUsdPlugin.h
#ifndef DATASMITHUSD_H
#define DATASMITHUSD_H

#include "USDIncludesStart.h"
#include "pxr/usd/usd/common.h"
#include "USDIncludesEnd.h"

#include <string>

PXR_NAMESPACE_OPEN_SCOPE

class FDatasmithUsd {
public:
    void Export(const UsdStageRefPtr& InStage);
};

PXR_NAMESPACE_CLOSE_SCOPE


#endif // DATASMITHUSDPLUGIN_H
