After cloning the repo:
### Synthetic Dataset Generation
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
