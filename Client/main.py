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
)
import os

def clear_terminal():
    """Clears the terminal screen."""
    os.system('cls' if os.name == 'nt' else 'clear')

# Player name and number of cards in hand
players_map = {
    "Player 1": 6,
    "Player 2": 5,
    "Player 3": 4,
    "Player 4": 2,
}

client_hand = ["Blue|Rev", "Red|Skip", "Green|PL2", "Black|Wild", "Black|Wild4", "Red|9"]
current_card = "Yellow|9"
turn = "Player1"


handerGameInit()

client_name = handleGetName()

#Lobby
while True:
    clear_terminal()
    waitingForLobby()
    time.sleep(3)
    # Check if the game has started
    if True:
        handleJoinedGame()
        break


# Game loop
while True:
    # Clear terminal
    clear_terminal()

    # Check if it's the player's turn
    if turn == client_name:
        print('SUDO UNO Game!')
        printOpponentsHand(players_map, client_name)
        printHand(client_hand)
        printTopcard(current_card)
        printTurn(turn, True)
        printOptions(["Play a card", "Draw a card", "Call UNO"])
        while True:
            player_choice = input("Enter your choice: ")
            # Process player choice
            if player_choice == "1":
                handlePlayerChoice1(player_choice, client_hand, client_name)
                # Clear terminal after player plays a card
                time.sleep(10)
                clear_terminal()
                
            # Process player choice draw a card
            if player_choice == "2":
                # Clear terminal after player plays a card
                #handlePlayerChoice2(player_choice, client_hand, client_name)
                time.sleep(10)
                clear_terminal()
        

            # Process player choice and check if they have only one card left
            if player_choice == "3":
                # Clear terminal after player plays a card
                #handlePlayerChoice3(player_choice, client_hand, client_name)
                time.sleep(10)
                clear_terminal()
        


    else:
        print('SUDO UNO Game!')
        printOpponentsHand(players_map, client_name)
        printHand(client_hand)
        printTopcard(current_card)
        printTurn(turn, False)
        printOptions(["Play a card", "Draw a card", "Call SUDO"])

        # Wait for game state change
        time.sleep(1)

    # Update game state
    #lil pause
    time.sleep(10)

