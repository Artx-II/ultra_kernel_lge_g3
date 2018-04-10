# DO NOT USE
# This script only works with KernelBuilding Essentials Program (By Artx)
# Current Folder
CURF=$(pwd)

cd ../../
source config.sh
cd $CURF

# Check Cross Compiler
if [ -f "$CROSSCOMPILE"gcc ]; then
export CROSS_COMPILE=$CROSSCOMPILE
export ARCH=$ARCH
echo -e "$GREEN Exported Building Tools Sucessfully"
else
echo -e "$RED Build Tools not found$RATT"
fi

# Done
echo -e "$GREEN Done$RATT"
