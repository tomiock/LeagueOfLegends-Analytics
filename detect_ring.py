import cv2
import numpy as np
from remove_terrain import hsv_to_bounds
from object_extraction import detect_circles

BLUE = (198.1, 90.4, 77.6)
RED = (352.4, 74.0, 75.3)

CENTER = (30, 35)


def detect_ring(image, center):
    hsv_image = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

    lower_red, upper_red = hsv_to_bounds(RED, 25)
    lower_blue, upper_blue = hsv_to_bounds(BLUE, 25)

    mask1 = cv2.inRange(hsv_image, lower_red, upper_red)
    mask2 = cv2.inRange(hsv_image, lower_blue, upper_blue)

    mask = mask1 + mask2

    circles = detect_circles(
        mask, radius=30, param1=200, param2=10, tolerance=3)
    if circles is not None:
        mask_result = np.zeros_like(image)
        for circle in circles[0]:
            x, y, r = circle
            center = (int(x), int(y))
            mask_result = cv2.circle(
                mask_result, center, int(r), (255, 255, 255), -1)
        return mask_result

    contours, _ = cv2.findContours(
        mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    contours = sorted(contours, key=cv2.contourArea, reverse=True)

    total_area = sum([cv2.contourArea(contour) for contour in contours])
    area, i = 0, 0
    while area < 0.80 * total_area:
        area += cv2.contourArea(contours[i])
        i += 1

    contours = contours[:i]

    mask = np.zeros_like(mask)
    cv2.drawContours(mask, contours, -1, 255, -1)

    mask = cv2.cvtColor(mask, cv2.COLOR_GRAY2BGR)

    centroids = []

    for contour in contours:
        if len(contour) >= 5:
            line = cv2.fitLine(contour, cv2.DIST_L2, 0, 0.01, 0.01)
            vx, vy, x, y = line

            p1, p2 = intersection_cnt_line(contour, line)

            middle = (p1 + p2) // 2

            vx, vy = vy, -vx
            x, y = middle
            perp_line = (vx, vy, x, y)

            p3, p4 = intersection_cnt_line(contour, perp_line)

            line1 = create_line(p1, p3)
            middle1 = (p1 + p3) // 2
            vx1, vy1, _, _ = line1
            perp_line1 = (vy1, -vx1, middle1[0], middle1[1])

            line2 = create_line(p1, p4)
            middle2 = (p1 + p4) // 2
            vx2, vy2, _, _ = line2
            perp_line2 = (vy2, -vx2, middle2[0], middle2[1])

            line3 = create_line(p2, p3)
            middle3 = (p2 + p3) // 2
            vx3, vy3, _, _ = line3
            perp_line3 = (vy3, -vx3, middle3[0], middle3[1])

            lines = [perp_line1, perp_line2, perp_line3]
            point = find_closest_point(lines)

            centroids.append(tuple(-point.astype(int)))

    if len(centroids) == 0:
        return mask

    closest_centroid = min(centroids, key=lambda x: (
        x[0] - center[0])**2 + (x[1] - center[1])**2)
    centroids.remove(closest_centroid)

    mask_result = np.zeros_like(mask)
    mask_result = cv2.circle(
        mask_result, closest_centroid, 30, (255, 255, 255), -1)

    for centroid in centroids:
        mask_result = cv2.circle(mask_result, centroid, 30, (0, 0, 0), -1)

    return mask_result


def line_from_params(params):
    vx, vy, x0, y0 = params
    A = vy
    B = -vx
    C = vx * y0 - vy * x0
    return A, B, C


def construct_design_matrix(lines):
    A_matrix = []
    for line in lines:
        A, B, _ = line_from_params(line)
        A_matrix.append([A, B])
    return np.array(A_matrix), np.array([line_from_params(line)[2] for line in lines])


def find_closest_point(lines):
    A_matrix, C_vector = construct_design_matrix(lines)

    point, _, _, _ = np.linalg.lstsq(A_matrix, C_vector, rcond=None)
    return point


def draw_line(image, line, color=None):
    if color is None:
        color = (0, 255, 255)

    vx, vy, x, y = line

    if vx == 0:
        cv2.line(image, (x, 0), (x, image.shape[0]), color, 1)
        return
    if vy == 0:
        cv2.line(image, (0, y), (image.shape[1], y), color, 1)
        return

    cols = image.shape[1]
    lefty = int((-x * vy / vx) + y)
    righty = int(((cols - x) * vy / vx) + y)
    cv2.line(image, (cols - 1, righty), (0, lefty), color, 1)


def random_rgb():
    return tuple(np.random.randint(0, 256, 3))


def create_line(p1, p2):
    x1, y1 = p1
    x2, y2 = p2

    vx = x2 - x1
    vy = y2 - y1

    x = (x1 + x2) // 2
    y = (y1 + y2) // 2

    return vx, vy, x, y


def intersection_cnt_line(contour, line):
    vx, vy, x, y = line
    distances = []

    for point in contour:
        x0, y0 = point[0]
        d = abs(vy * (x0 - x) - vx * (y0 - y)) / (vx**2 + vy**2)**0.5
        distances.append(d)

    distances = np.array(distances)
    p1 = contour[np.argmin(distances)][0]
    p2 = contour[np.argmax(distances)][0]

    return p1, p2


if __name__ == '__main__':
    img = cv2.imread('poppy_detected.png')
    mask = detect_ring(img, CENTER)

    img = cv2.bitwise_and(img, mask)

    cv2.imshow('img', img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
