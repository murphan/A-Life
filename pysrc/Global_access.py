"""
This file has Global variables and information that can be acessed by the entire application.
It requires access to the mutex in order to update as there are threads in the application
"""

from threading import Thread, Lock

# This is only set to be 1 when the program is exiting. Do not use unless ending execution
EXIT = 0

# Colors
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
GREEN = '#0c871b'
YELLOW = '#fafa16'

# Environment factors
speed = 1
temperature = 0
oxygen = 0
light = 0

height = 1
width = 1
running = False

CLICK_TYPE = "Organism"
ENVIRONMENT_GRID = []

SCREEN = None
WINDOW_HEIGHT = 770
WINDOW_WIDTH = 1500
environment_size = 0, 0

mutex = Lock()


def define_grid(width, height):
    """
    This will set the correct size of the environment window reference grid

    :param width: width of the environment
    :param type: int, float

    :param height: height of the environment
    :param type: int, float
    """
    mutex.acquire()
    global ENVIRONMENT_GRID
    ENVIRONMENT_GRID = [[0 for x in range(int(height))] for y in range(int(width))]
    mutex.release()


def update_grid(x, y):
    mutex.acquire()
    global ENVIRONMENT_GRID
    ENVIRONMENT_GRID[x][y] = 1
    mutex.release()


def change_speed(new_speed):
    mutex.acquire()
    global speed
    speed = new_speed
    mutex.release()


def change_temperature(new_temp):
    mutex.acquire()
    global temperature
    temperature = new_temp
    mutex.release()


def change_light(new_light):
    mutex.acquire()
    global light
    light = new_light
    mutex.release()


def change_oxygen(new_oxygen):
    mutex.acquire()
    global oxygen
    oxygen = new_oxygen
    mutex.release()


def change_click_type(new_click_type):
    mutex.acquire()
    global CLICK_TYPE
    CLICK_TYPE = new_click_type
    mutex.release()


def set_screen(screen):
    global SCREEN
    SCREEN = screen

def set_environment_size(width, height):
    mutex.acquire()
    global environment_size
    environment_size = width, height
    mutex.release()
