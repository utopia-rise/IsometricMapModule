# Create tiles

Tile are represented by `IsometricPositionable` node. So you first need to create a scene with an `IsometricPositionable`
root node, as below:

![create-tile-gif]

You can change the `IsometricPositionable` size from node inspector:

![change-tile-size-gif]

Then add your textures like you would do for any 2D godot node, your texture should not go out of tile boundaries:

![add-tile-texture-gif]

Once you registered your tile scene, you can take advantage of godot's scene inheritance mechanism. This way you can
create a new tile from an existing one.  
This way, if you modify the base tile, it will modify all derived tiles.

![inherit-tile-gif]

You can also add 3D physics node as child of your tile to provide 3D world collisions:

![tile-add-physics-gif]

[create-tile-gif]: assets/creating-tile/create-tile.gif
[change-tile-size-gif]: assets/creating-tile/chante-tile-size.gif
[add-tile-texture-gif]: assets/creating-tile/add-tile-texture.gif
[inherit-tile-gif]: assets/creating-tile/inherit-tile.gif
[tile-add-physics-gif]: assets/creating-tile/tile-add-physics.gif