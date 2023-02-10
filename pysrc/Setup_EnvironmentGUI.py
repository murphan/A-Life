import os
from multiprocessing import Process
import sys
import socket
import pygame
import random

import Control_EnvironmentGUI
import Setup_settingsGUI


BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
WINDOW_HEIGHT = 770
WINDOW_WIDTH = 1500
CLOCK = None
SCREEN = None


class SetupEnvironment:
    """
    This will set up the actual environment for the simulation to be displayed in
    """
    def __init__(self):
        global CLOCK, SCREEN
        pygame.init()
        self.SCREEN = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
        SCREEN = self.SCREEN
        CLOCK = pygame.time.Clock()
        self.SCREEN.fill(WHITE)

        self.createButtons()
        self.environment_size = (WINDOW_WIDTH / 10, (WINDOW_HEIGHT - 50) / 10)
        Control_EnvironmentGUI.EnvironmentControl.define_grid(self, self.environment_size[0], self.environment_size[1])

    def drawGrid(self):
        """
        This will draw the grid of the environment and initialize a two-dimensional array
        """
        block_size = 10
        for x in range(0, WINDOW_WIDTH, block_size):
            for y in range(0, WINDOW_HEIGHT - 50, block_size):
                rect = pygame.Rect(x, y, block_size, block_size)
                pygame.draw.rect(self.SCREEN, BLACK, rect, 1)

    def createButtons(self):
        # The three buttons that are on the environment screen
        self.settings_button = Button(self.SCREEN, (1420, 730), "Settings", (255, 255, 0))
        self.pause_button = Button(self.SCREEN, (1356, 730), "Pause", (255, 0, 0))
        self.play_button = Button(self.SCREEN, (1307, 730), "Play", (0, 255, 0))

    def add_organism_display(self, formatted_string):
        """
        This will display the formatted string of organism information at the bottom of the screen

        :param formatted_string: formatted string with the organism information
        :param type: string
        """

        test_string = f"ID: {random.randint(111111111, 999999999)}  \
          Body = U2FsdGVkX1/DX9ztNOyA+E0ztNTyMtLx/bKrIzZlN/E=  \
          width = 5  \
          height = 5  \
          energy = 100  \
          age = 122  \
          x = 54  \
          y = 81"

        font = pygame.font.SysFont("freesansbold.ttf", 22)
        text_options = font.render(test_string, True, (0, 0, 0))
        textRect = text_options.get_rect()
        textRect.center = (670, 745)
        self.SCREEN.blit(text_options, textRect)


class Button:
    """
    The button class which defines the buttons in the environment
    """
    def __init__(self, screen, position, text, color):
        font = pygame.font.SysFont("Arial", 25)
        text_options = font.render(text, True, (0, 0, 0))
        x, y, w, h = text_options.get_rect()
        x, y = position
        self.button_rect = pygame.draw.rect(screen, color, (x, y, w, h))
        screen.blit(text_options, self.button_rect)

    def clicked(self):
        """
        Returns True if you clicked on the specified button being checked
        """
        x, y = pygame.mouse.get_pos()
        return True if self.button_rect.collidepoint(x, y) else False

    def mouse_click(self, event):
        """
        Checks if you click the mouse button and then if it's on the button
        """
        if event.type == pygame.MOUSEBUTTONDOWN:
            if pygame.mouse.get_pressed()[0]:
                return True if self.clicked() else False
