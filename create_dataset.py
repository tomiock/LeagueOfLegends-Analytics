import io

from PIL import Image, ImageDraw
from tqdm import tqdm
import csv
import string
import os
import random

RADIUS_CIRCLE = 56
COLOR_BLUE = '#1c7cce'
COLOR_RED = '#cc122b'
CIRCLE_WIDTH = 4
OFFSET = 10
RESIZE_CHAMPION = 40
IMAGE_SIZE = (640, 640)
DATASET_SIZE = 100
NUM_CHAMPIONS = 167

MAP_FILE = 'reference_images/maps/MAP.png'
CHAMPIONS_DIR = 'reference_images/champions'
CSV_FILE = 'reference_images/champion_names.csv'


def create_champion_icon(image_path: str, color: str):
    radius = RADIUS_CIRCLE  # Set the radius of the circle
    circumference_width = CIRCLE_WIDTH

    if color == 'blue':
        color = '#1c7cce'
    elif color == 'red':
        color = '#cc122b'
    else:
        raise TypeError("Color should be either 'red' or 'blue'")

    image = Image.open(image_path)
    image = image.convert("RGBA")

    width, height = image.size

    # Calculate the center of the image
    center_x = width // 2
    center_y = height // 2

    mask = Image.new('L', (width, height), 0)
    draw = ImageDraw.Draw(mask)

    draw.ellipse((center_x - radius, center_y - radius, center_x + radius, center_y + radius), fill=255)

    result = Image.new('RGBA', (width, height), (255, 255, 255, 0))
    result.paste(image, (0, 0), mask=mask)

    draw = ImageDraw.Draw(result)
    draw.ellipse((center_x - radius, center_y - radius, center_x + radius, center_y + radius), outline=color,
                 width=circumference_width)

    return result


def paste_on_larger_image(champion_image, larger_image, path_filename, label: str):
    OFFSET = larger_image.width // 7
    offset_x = OFFSET
    offset_y = OFFSET

    random_x = random.randint(offset_x, larger_image.width - offset_x)
    random_y = random.randint(offset_y, larger_image.height - offset_y)

    resized_result_image = champion_image.resize((RESIZE_CHAMPION, RESIZE_CHAMPION))
    larger_image.paste(resized_result_image, (random_x, random_y), resized_result_image)

    with io.open(path_filename, 'a') as file:
        annotation = generate_yolov5_annotations((larger_image.width, larger_image.height),
                                                 int(resized_result_image.width / 2), (random_x + 20, random_y + 20))
        file.write(f'{label} {annotation}\n')

    return larger_image


def encode_number_to_letters(n):
    if n < 0:
        raise ValueError("Input must be a non-negative integer.")

    letters = string.ascii_lowercase
    base = len(letters)
    result = []

    while n >= base:
        remainder = n % base
        letter = letters[remainder]
        result.append(letter)
        n = n // base

    result.append(letters[n])
    result.reverse()

    return "".join(result)


def generate_yolov5_annotations(image_size: tuple[int, int], circle_radius: int, circle_center: tuple[int, int]) -> str:
    width, height = image_size
    center_x, center_y = circle_center

    top_left_x = max(0, center_x - circle_radius)
    top_left_y = max(0, center_y - circle_radius)
    bottom_right_x = min(width, center_x + circle_radius)
    bottom_right_y = min(height, center_y + circle_radius)

    bbox_width = (bottom_right_x - top_left_x) / width
    bbox_height = (bottom_right_y - top_left_y) / height
    bbox_center_x = (top_left_x + bottom_right_x) / (2 * width)
    bbox_center_y = (top_left_y + bottom_right_y) / (2 * height)

    return f"{bbox_center_x:.7f} {bbox_center_y:.6f} {bbox_width:.6f} {bbox_height:.6f}"


def get_name_from_csv_row(filename, row_number):
    with open(filename, 'r', newline='') as file:
        reader = csv.reader(file)
        rows = list(reader)
        selected_row_index = row_number
        selected_row = rows[selected_row_index]

    return selected_row[1]


def generate_image(number: int):
    path = 'dataset'

    encoded = encode_number_to_letters(number)
    file_name_joined_images = os.path.join(path + '/images', encoded) + '.png'
    file_name_joined_labels = os.path.join(path + '/labels', encoded) + '.txt'
    open(file_name_joined_labels, 'w').close()

    map_image = Image.open(MAP_FILE)
    map_image = map_image.convert("RGBA")

    for _ in range(5):
        blue, red = 0, 0
        while blue == red:
            blue = random.randint(1, NUM_CHAMPIONS)
            red = random.randint(1, NUM_CHAMPIONS)

        blue = get_name_from_csv_row(CSV_FILE, blue)
        red = get_name_from_csv_row(CSV_FILE, red)

        image_blue = f'{CHAMPIONS_DIR}/{str(blue)}.png'
        image_red = f'{CHAMPIONS_DIR}/{str(red)}.png'

        image_blue = create_champion_icon(image_blue, color='blue')
        image_red = create_champion_icon(image_red, color='red')

        map_image = paste_on_larger_image(image_blue, map_image, file_name_joined_labels, str(blue))
        map_image = paste_on_larger_image(image_red, map_image, file_name_joined_labels, str(red))

    map_image.save(file_name_joined_images)


if __name__ == '__main__':
    if not os.path.exists('dataset'):
        print('Error: Folder "dataset" does not exist. Please create it first.')
        exit(-1)
    if not os.path.exists('dataset/images'):
        os.makedirs('dataset/images')
    if not os.path.exists('dataset/labels'):
        os.makedirs('dataset/labels')

    if not os.path.exists(CHAMPIONS_DIR):
        print('Error: Folder "champions" does not exist. Please create it first.')
        exit(-1)

    for i in tqdm(range(DATASET_SIZE - 1)):
        generate_image(i)
