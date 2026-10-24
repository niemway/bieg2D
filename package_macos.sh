#!/bin/bash
# Nicholas Way game engine macOS packaging script
#
# Builds the engine and wraps it, the SDL frameworks and a game's resources/
# folder into a double-clickable .app bundle.
#
# usage: ./package_macos.sh [options]
#   -n NAME        app name (default: game_title from resources/game.config)
#   -r DIR         resources folder to bundle (default: ./resources)
#   -b BUNDLE_ID   bundle identifier (default: niemway.bieg2d.<name>)
#   -v VERSION     version string (default: 1.0)
#   -i ICON        .icns file to use as the app icon
#   -s IDENTITY    codesign identity, e.g. "Developer ID Application: ..."
#                  (default: ad-hoc signing, fine for running on this Mac)
#   -o DIR         output folder (default: ./build)

set -euo pipefail

ROOT="$(cd "$(dirname "$0")" && pwd)"
RESOURCES="$ROOT/resources"
OUT_DIR="$ROOT/build"
VERSION="1.0"
APP_NAME=""
BUNDLE_ID=""
ICON=""
IDENTITY="-"

while getopts "n:r:b:v:i:s:o:h" opt; do
    case "$opt" in
        n) APP_NAME="$OPTARG" ;;
        r) RESOURCES="$(cd "$OPTARG" && pwd)" ;;
        b) BUNDLE_ID="$OPTARG" ;;
        v) VERSION="$OPTARG" ;;
        i) ICON="$OPTARG" ;;
        s) IDENTITY="$OPTARG" ;;
        o) OUT_DIR="$OPTARG" ;;
        *) sed -n '2,17p' "$0" | sed 's/^# \{0,1\}//'; exit 1 ;;
    esac
done

if [ ! -f "$RESOURCES/game.config" ]; then
    echo "error: $RESOURCES/game.config missing" >&2
    exit 1
fi

if [ -z "$APP_NAME" ]; then
    APP_NAME="$(plutil -extract game_title raw -o - "$RESOURCES/game.config" 2>/dev/null || echo "bieg2D Game")"
fi
if [ -z "$BUNDLE_ID" ]; then
    BUNDLE_ID="niemway.bieg2d.$(echo "$APP_NAME" | tr -cd '[:alnum:]' | tr '[:upper:]' '[:lower:]')"
fi

mkdir -p "$OUT_DIR"
OUT_DIR="$(cd "$OUT_DIR" && pwd)"
DERIVED_DATA="$OUT_DIR/derived_data"
APP="$OUT_DIR/$APP_NAME.app"

# build a universal release binary that looks for frameworks inside the bundle
echo "==> building game_engine (Release, arm64 + x86_64)"
xcodebuild \
    -project "$ROOT/game_engine.xcodeproj" \
    -scheme game_engine \
    -configuration Release \
    -derivedDataPath "$DERIVED_DATA" \
    ARCHS="arm64 x86_64" \
    ONLY_ACTIVE_ARCH=NO \
    LD_RUNPATH_SEARCH_PATHS="@executable_path/../Frameworks" \
    CODE_SIGNING_ALLOWED=NO \
    -quiet build

BINARY="$DERIVED_DATA/Build/Products/Release/game_engine"

# assemble the bundle
echo "==> assembling $APP"
rm -rf "$APP"
mkdir -p "$APP/Contents/MacOS" "$APP/Contents/Frameworks" "$APP/Contents/Resources"

cp "$BINARY" "$APP/Contents/MacOS/game_engine"

for framework in SDL2 SDL2_image SDL2_ttf SDL2_mixer; do
    ditto "$ROOT/SDL_Frameworks/$framework.framework" "$APP/Contents/Frameworks/$framework.framework"
    rm -rf "$APP/Contents/Frameworks/$framework.framework/Headers" \
           "$APP/Contents/Frameworks/$framework.framework/Versions/A/Headers"
done

rsync -a --exclude ".DS_Store" "$RESOURCES/" "$APP/Contents/Resources/resources/"

ICON_KEY=""
if [ -n "$ICON" ]; then
    cp "$ICON" "$APP/Contents/Resources/AppIcon.icns"
    ICON_KEY="<key>CFBundleIconFile</key><string>AppIcon</string>"
fi

cat > "$APP/Contents/Info.plist" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleName</key><string>$APP_NAME</string>
    <key>CFBundleDisplayName</key><string>$APP_NAME</string>
    <key>CFBundleExecutable</key><string>game_engine</string>
    <key>CFBundleIdentifier</key><string>$BUNDLE_ID</string>
    <key>CFBundlePackageType</key><string>APPL</string>
    <key>CFBundleShortVersionString</key><string>$VERSION</string>
    <key>CFBundleVersion</key><string>$VERSION</string>
    <key>CFBundleInfoDictionaryVersion</key><string>6.0</string>
    <key>LSMinimumSystemVersion</key><string>14.0</string>
    <key>LSApplicationCategoryType</key><string>public.app-category.games</string>
    <key>NSHighResolutionCapable</key><true/>
    $ICON_KEY
</dict>
</plist>
EOF
plutil -lint -s "$APP/Contents/Info.plist"

# sign frameworks first, then the app (hardened runtime only for a real identity)
echo "==> signing (identity: $IDENTITY)"
SIGN_OPTS=(--force --timestamp=none --sign "$IDENTITY")
if [ "$IDENTITY" != "-" ]; then
    SIGN_OPTS=(--force --timestamp --options runtime --sign "$IDENTITY")
fi
for framework in "$APP"/Contents/Frameworks/*.framework; do
    codesign "${SIGN_OPTS[@]}" "$framework"
done
codesign "${SIGN_OPTS[@]}" --entitlements "$ROOT/game_engine.entitlements" "$APP"
codesign --verify --deep --strict "$APP"

echo "==> done: $APP"
if [ "$IDENTITY" != "-" ]; then
    echo "to distribute, notarize it:"
    echo "  ditto -c -k --keepParent \"$APP\" \"$OUT_DIR/$APP_NAME.zip\""
    echo "  xcrun notarytool submit \"$OUT_DIR/$APP_NAME.zip\" --keychain-profile <profile> --wait"
    echo "  xcrun stapler staple \"$APP\""
fi
