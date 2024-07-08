import urllib.request as request
import os
import tqdm

VERSION = '14.13.1'

champion_names = ["Aatrox",
                  "Ahri",
                  "Akali",
                  "Akshan",
                  "Alistar",
                  "Amumu",
                  "Anivia",
                  "Annie",
                  "Aphelios",
                  "Ashe",
                  "AurelionSol",
                  "Azir",
                  "Bard",
                  "Belveth",
                  "Blitzcrank",
                  "Brand",
                  "Braum",
                  "Briar",
                  "Caitlyn",
                  "Camille",
                  "Cassiopeia",
                  "Chogath",
                  "Corki",
                  "Darius",
                  "Diana",
                  "DrMundo",
                  "Draven",
                  "Ekko",
                  "Elise",
                  "Evelynn",
                  "Ezreal",
                  "Fiddlesticks",
                  "Fiora",
                  "Fizz",
                  "Galio",
                  "Gangplank",
                  "Garen",
                  "Gnar",
                  "Gragas",
                  "Graves",
                  "Gwen",
                  "Hecarim",
                  "Heimerdinger",
                  "Hwei",
                  "Illaoi",
                  "Irelia",
                  "Ivern",
                  "Janna",
                  "JarvanIV",
                  "Jax",
                  "Jayce",
                  "Jhin",
                  "Jinx",
                  "KSante",
                  "Kaisa",
                  "Kalista",
                  "Karma",
                  "Karthus",
                  "Kassadin",
                  "Katarina",
                  "Kayle",
                  "Kayn",
                  "Kennen",
                  "Khazix",
                  "Kindred",
                  "Kled",
                  "KogMaw",
                  "Leblanc",
                  "LeeSin",
                  "Leona",
                  "Lillia",
                  "Lissandra",
                  "Lucian",
                  "Lulu",
                  "Lux",
                  "Malphite",
                  "Malzahar",
                  "Maokai",
                  "MasterYi",
                  "Milio",
                  "MissFortune",
                  "Mordekaiser",
                  "Morgana",
                  "Nami",
                  "Naafiri",
                  "Nasus",
                  "Nautilus",
                  "Neeko",
                  "Nidalee",
                  "Nilah",
                  "Nocturne",
                  "Nunu",
                  "Olaf",
                  "Orianna",
                  "Ornn",
                  "Pantheon",
                  "Poppy",
                  "Pyke",
                  "Qiyana",
                  "Quinn",
                  "Rakan",
                  "Rammus",
                  "RekSai",
                  "Rell",
                  "Renata",
                  "Renekton",
                  "Rengar",
                  "Riven",
                  "Rumble",
                  "Ryze",
                  "Samira",
                  "Sejuani",
                  "Senna",
                  "Seraphine",
                  "Sett",
                  "Shaco",
                  "Shen",
                  "Shyvana",
                  "Singed",
                  "Sion",
                  "Sivir",
                  "Skarner",
                  "Smolder",
                  "Sona",
                  "Soraka",
                  "Swain",
                  "Sylas",
                  "Syndra",
                  "TahmKench",
                  "Taliyah",
                  "Talon",
                  "Taric",
                  "Teemo",
                  "Thresh",
                  "Tristana",
                  "Trundle",
                  "Tryndamere",
                  "TwistedFate",
                  "Twitch",
                  "Udyr",
                  "Urgot",
                  "Varus",
                  "Vayne",
                  "Veigar",
                  "Velkoz",
                  "Vex",
                  "Vi",
                  "Viego",
                  "Viktor",
                  "Vladimir",
                  "Volibear",
                  "Warwick",
                  "MonkeyKing",
                  "Xayah",
                  "Xerath",
                  "XinZhao",
                  "Yasuo",
                  "Yone",
                  "Yorick",
                  "Yuumi",
                  "Zac",
                  "Zed",
                  "Zeri",
                  "Ziggs",
                  "Zilean",
                  "Zoe",
                  "Zyra"]


for name in tqdm.tqdm(champion_names):
    url = f'https://ddragon.leagueoflegends.com/cdn/{VERSION}/img/champion/{name}.png'
    request.urlretrieve(url, os.path.join(f'./champions/{name}.png'))

# rename image to Wukong because Riot idk what its doing
if os.path.exists('./champions/MonkeyKing.png'):
    os.rename('./champions/MonkeyKing.png', './champions/Wukong.png')

if champion_names.index('MonkeyKing') != -1:
    champion_names[champion_names.index('MonkeyKing')] = 'Wukong'

# create CSV file that maps champion names to their index
with open('champion_names.csv', 'w') as f:
    f.write('index,name\n')
    for idx, name in enumerate(champion_names):
        f.write(f'{idx+1},{name}\n')
