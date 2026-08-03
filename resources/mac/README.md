# macOS app icon

| File | Purpose |
|------|---------|
| `openrct2.icon` | Icon source: an Apple [Icon Composer](https://developer.apple.com/icon-composer/) package (like `.iconset`, but for the layered "Liquid Glass" icons introduced with macOS 26). Contains `icon.json` (layer/material manifest) and the layer artwork. |
| `Assets.car` | Compiled asset catalog, prebuilt from `openrct2.icon`. Bundled into `OpenRCT2.app` and referenced by `CFBundleIconName`; macOS 26 and later renders the layered Liquid Glass icon from it. Prebuilt so building OpenRCT2 does not require Xcode 26. |
| `openrct2.icns` | Flat icon for macOS 15 and earlier, referenced by `CFBundleIconFile`. Extracted from `Assets.car`, so it is the same artwork. |
| `openrct2.pxd` | Editable Pixelmator Pro master for the layer artwork. |
| `Archived/` | The previous (Big Sur era) icon and its vector source. |

## Regenerating after artwork changes

Edit `openrct2.icon` in Icon Composer (keep layers as PNG; SVG layers do not
receive the glass material), then from `resources/mac`:

```
xcrun actool openrct2.icon --compile out --app-icon openrct2 \
    --output-partial-info-plist out/partial.plist \
    --platform macosx --target-device mac \
    --minimum-deployment-target 11.0 --errors --warnings
cp out/Assets.car Assets.car
iconutil -c icns Assets.car openrct2 -o openrct2.icns
rm -r out
```

Requires Xcode 26 or later for `actool` (build machines do not need it; both
artifacts are checked in). The `--app-icon` value, the `.icon` basename, and
`CFBundleIconName` in `distribution/macos/Info.plist` must all stay `openrct2`.
