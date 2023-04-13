import os
from threading import Thread
from multiprocessing import Process
import sys
import socket
from tkinter import messagebox
# Hides a console message from the pygame module
os.environ['PYGAME_HIDE_SUPPORT_PROMPT'] = '1'
import pygame

import Control_EnvironmentGUI
import Setup_EnvironmentGUI
import Setup_settingsGUI
import Global_access
import receive_message

class Management:
    def __init__(self, *args, **kwargs):
        self.EnvironmentGui = Setup_EnvironmentGUI.SetupEnvironment()
        self.EnvironmentControl = Control_EnvironmentGUI.EnvironmentControl()

        self.create_connection()

        # TODO call whenever new connection is established
        self.send_init_message()

    def create_connection(self):
        # Set up the socket connection to the c++ application
        connected = False
        while not connected:
            try:
                host = socket.gethostname()
                port = self.read_port()
                self.conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.conn.connect((host, port))
                connected = True
            except socket.error:
                continue

    def start_receiver(self):
        self.thread = Thread(target=receive_message.decode_message, args=(self, self.conn,))
        self.thread.start()

    def send_init_message(self):
        self.EnvironmentControl.send_message(self.conn, "init")

    def main_loop(self):
        while True:
            self.EnvironmentGui.clear_screen()
            if Global_access.EXIT == 1:
                self.conn.close()
                pygame.quit()
                sys.exit()
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    thread = Thread(target=self.exiting)
                    thread.start()
                if event.type == pygame.MOUSEBUTTONDOWN:
                    if self.EnvironmentGui.settings_button.mouse_click(event):  # Show the settings window
                        thread = Thread(target=self.open_settings)
                        thread.start()
                    elif self.EnvironmentGui.play_button is not None and \
                            self.EnvironmentGui.play_button.mouse_click(event):  # start button was clicked
                        self.EnvironmentControl.start(self.conn)
                        self.EnvironmentGui.createButtons()
                    elif self.EnvironmentGui.pause_button is not None and \
                            self.EnvironmentGui.pause_button.mouse_click(event):  # pause button was clicked
                        self.EnvironmentControl.stop(self.conn)
                        self.EnvironmentGui.createButtons()
                    else:  # Display on the environment that a square was clicked
                        self.EnvironmentControl.square_clicked(event, self.EnvironmentGui, self.conn)

            pygame.display.update()

    def exiting(self):
        confirm = messagebox.askokcancel(title="Quit?", message="Are you sure that you want to Quit?")
        if confirm:
            Global_access.EXIT = 1
        else:
            return

    def read_port(self):
        f = open("../config.txt", "r")
        return int(f.read())

    def open_settings(self):
        settings = Setup_settingsGUI.SetupSettings()
        settings.start(self)


if __name__ == "__main__":
    starter = Management()
    starter.start_receiver()
    starter.main_loop()
