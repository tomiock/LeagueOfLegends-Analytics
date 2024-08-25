# Computer Vision to Analyze Pro League Games

Based primarily in OpenCV by feeding images or video

## C++

Detects circles to run champion recognition on them. Since champion icons can be on top of each other to the point of making some of them unrecognizable even for humans. Thus, we need to work with all the knowledge that we can.

There are 3 norms that dictate which champ icons are on top on the others. From how on we are going to refer to this aspect of the code/game **Icon Priority**: 
1. Red icons over Blue icons
2. Supports over everyone else and so on:
    - Support
    - ADC
    - Mid
    - Jungler
    - Top
3. If some is recalling moves at the bottom. Some not recalling is always on top of some not recalling. Thus, this rules ignores the other two.

Using this we first try to recognize the support if an icon is on top of another and so on. The goal is keep reducing the search space with the information that is gathered. The most simple example of this is trying to recognize the blue champs only inside of blue circles and the red champs with the red circles.

### Installation from source
1. Clone
2. Install dependencies
3. Run `make` to build the executable. By default is called `opencv_lol` and is located in the project directory.


### Execution/Build with Flakes (Nix)

Build:
```shell
nix build github:tomiock/LeagueOfLegends-Analytics
```
and install for the local user:
```shell
nix profile install .
```
Execute this commands on the project directory.

The repo can also be cloned. Use `nix flake show` on the repo directory to see the available outputs. There is a shell available in which `make` can be executed to build directly without using nix. Doing this will generate the `compile_commands.json` correctly, unlike `nix build`.

## Python

### Champion Recognition

Steps:
1. Remove unnecessary colors/info (terrain, river, shadows...)
2. Detect Circles
3. Match the icon inside of circle to a champion

About 80%-90% accuracy.

### YOLO Synthetic Dataset Creation

>  Deprecated for a more simple solution. I do not know what to do with this.

1. Request the champion images from the Riot API:
```
python3 reference_images/request_champion_square_images.py
```
2. Generate the dataset.
The default config values are already setup at the beginning of the Python file in the form of constans, feel free to change them. Just run the file:
```
python3 create_dataset.py
```
The `dataset` folder can be created easily with `mkdir dataset`.

#### What it does have:
- 5 champs in each time located in the boundaries of the map
- each time has its color
- the champions icons have by default a realistic size on the map

#### Missing features (for now):
- no fog of war
- no dynamic or different turret or camps status (all camps are alive and turrents at full health)
- no minions
- no teamfights (all champs are not together in a close spot)
