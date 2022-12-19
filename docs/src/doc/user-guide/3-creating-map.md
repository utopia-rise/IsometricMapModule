# Creating Map

In order to create a map, create a new scene with `IsometricMap` as root node. Then you can set its size, as you can do
with classic `IsometricPositionable`: 

![create-map-gif]

Then, you should assign a `PositionableSet` resource to your map, using node inspector:

![assign-positionable-set-gif]

## Edition modes

There are several edition modes to help you create maps:
- Paint
- Drag & Drop
- Select

*Note that <kbd>Ctrl</kbd> (or <kbd>⌘ Command</kbd>)  + <kbd>Z</kbd> is available.*

### Paint

In `paint` mode, you will place positionable when clicking or maintaining left click:  

![paint-mod-gif]

### Drag & drop

In `drag and drop` mode, you will place positionables by maintaining left click and drawing a rectangle:  

![drag-and-drop-mod-gif]

### Select

In `select` mode, you can select one or multiple positionables. To select several maintain <kbd>Ctrl</kbd>
(or <kbd>⌘ Command</kbd>).  
To select all elements in map, use <kbd>Ctrl</kbd> (or <kbd>⌘ Command</kbd>) + <kbd>A</kbd>

![select-mod-gif]

Note that you can delete selected positionables on map by using <kbd>⌫</kbd>.  

![delete-tiles-gif]

### None

In none mode, no input is forwarded to the plugin, in this mode you can use classic godot commands, like saving the
scene.

## Moving and rotating edition grid

When you edit map, selection, painting and drag & drop are bound to the edition grid position.  
You can move up and down this grid by using <kbd>↑</kbd> and <kbd>↓</kbd> keyboard arrows.  
You can switch its normal axis by using <kbd>←</kbd> and <kbd>→</kbd> arrows.  

![edition-grid-moves-gif]

## Limit displayed tiles

In order to debug your map, you can limit tiles you display on an axis. You can limit on all axis using two plans, one
for minimum limit, one for maximum.

Here are commands to limit on axis:  
- X: <kbd>Ctrl</kbd> (or <kbd>⌘ Command</kbd>) + <kbd>1</kbd> + scroll
- Y: <kbd>Ctrl</kbd> (or <kbd>⌘ Command</kbd>) + <kbd>2</kbd> + scroll
- Z: <kbd>Ctrl</kbd> (or <kbd>⌘ Command</kbd>) + <kbd>3</kbd> + scroll

Those will set the limit on maximum, if you want to set minimum limit maintain <kbd>⇧ Shift</kbd> additionally.  

![limit-view-gif]

[create-map-gif]: ./assets/creating-map/create-map.gif
[assign-positionable-set-gif]: ./assets/creating-map/assign-positionable-set.gif
[paint-mod-gif]: ./assets/creating-map/paint-mod.gif
[drag-and-drop-mod-gif]: ./assets/creating-map/drag-and-drop-mod.gif
[select-mod-gif]: ./assets/creating-map/select-mod.gif
[delete-tiles-gif]: ./assets/creating-map/delete-tiles.gif
[edition-grid-moves-gif]: ./assets/creating-map/edition-grid-moves.gif
[limit-view-gif]: ./assets/creating-map/limit-view.gif