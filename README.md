## Synthetic Dataset Generation for Champion Recognition

> IMPORTANT: Currently the annotations for the dataset are in the `YOLO v5` style.

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
