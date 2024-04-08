# import statements
import socket
import sys
import time
from utils.logger import (
    printOpponentsHand,
    printHand,
    printTopcard,
    printTurn,
    printOptions,
    handlePlayerChoice1,
    handleGetName,
    waitingForLobby,
    handleJoinedGame,
    handerGameInit,
    handleGetServer,
)
from utils.connection import Connection

import os


def clear_terminal():
    """Clears the terminal screen."""
    os.system("cls" if os.name == "nt" else "clear")


# Player name and number of cards in hand
players_map = {
    "Player 1": 6,
    "Player 2": 5,
    "Player 3": 4,
    "Player 4": 2,
}

client_hand = [
    "Blue|Rev",
    "Red|Skip",
    "Green|PL2",
    "Black|Wild",
    "Black|Wild4",
    "Red|9",
]
current_card = "Yellow|9"
turn = "Player1"


def lobby_loop():
    # Lobby
    while True:
        clear_terminal()
        waitingForLobby()
        # Check if the game has started
        if True:
            handleJoinedGame()
            break


def game_loop(turn, client_name, players_map, client_hand, current_card):
    # Game loop
    while True:
        # Clear terminal
        clear_terminal()

        # Check if it's the player's turn
        if turn == client_name:
            print("SUDO UNO Game!")
            printOpponentsHand(players_map, client_name)
            printHand(client_hand)
            printTopcard(current_card)
            printTurn(turn, True)
            printOptions(["Play a card", "Draw a card", "Call UNO"])
            player_choice = input("Enter your choice: ")

            handlePlayerChoice(player_choice, client_hand, client_name)
            # Process player choice
            if player_choice == "1":
                # Clear terminal after player plays a card
                time.sleep(10)
                clear_terminal()

        else:
            print("SUDO UNO Game!")
            printOpponentsHand(players_map, client_name)
            printHand(client_hand)
            printTopcard(current_card)
            printTurn(turn, False)
            printOptions(["Play a card", "Draw a card", "Call SUDO"])

            # Wait for game state change
            time.sleep(1)

        # Update game state
        # lil pause
        time.sleep(10)


if __name__ == "__main__":
    handerGameInit()

    ip, port = handleGetServer()
    connection = Connection(ip, port)
    # print welcome message from server

    client_name = handleGetName()
    client_name = 'name = "' + client_name + '"'
    connection.send(client_name)

    # lobby_loop()
    # game_loop(turn, client_name, players_map, client_hand, current_card)
