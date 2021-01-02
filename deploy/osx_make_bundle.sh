#echo $1 $2

BUILD=$1
APP=$2
VER=$3

PACKAGE="$APP.app"
SRC="$BUILD/$PACKAGE"
DEST="$HOME/Documents/$APP-MAKE"
DATE=$(date +%d_%m_%y)
[ -z "$VER" ] && VER=$(sw_vers -productVersion)

if [ ! -d "$SRC" ]; then
    echo "Package $SRC does not exist"
    exit
fi

DMG="$DEST-$VER-${DATE}.dmg"
DISTRIBUTE="/Volumes/VMware\ Shared\ Folders/WORK/$DMG"
DEPLOYTOOL="/usr/local/Cellar/qt/5.14.2/bin/macdeployqt"

echo "Make deploy $PACKAGE ver:$VER"
$DEPLOYTOOL "$SRC"
rm -rf "$DEST"
mkdir -p "$DEST"
mv -f "$SRC" "$DEST/$PACKAGE"

echo "Make file $DMG"
hdiutil create "$DMG" -ov -volname "$APP-$VER-$DATE" -fs HFS+ -srcfolder "$DEST"

rm -rf "$DEST"

#cp $DMG $DISTRIBUTE
