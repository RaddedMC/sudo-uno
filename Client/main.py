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
    handlePlayerChoice,
    handleGetName,
    waitingForLobby,
    handleJoinedGame,
    handerGameInit,
    handleGetServer,
    printUI,
)
from utils.connection import Connection 

import os


def clear_terminal():
    """Clears the terminal screen."""
    os.system("cls" if os.name == "nt" else "clear")


# Player and game state
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
client_name = "Player1"


def lobby_loop():
    dots = 0
    i = 0 #temp variable
    # Lobby Loop (checks server every itteration to see if it should start the game)
    while True:
        clear_terminal()
        
        dots = waitingForLobby(dots)
        # Check if the game has started
        #if con.lobby() returns True handleJoining a game
        #con.lobby() returns a boolean of wether or not the client is in a game with 4 players
        #nPlayers = con.lobby()
        nPlayers = 0
        print(f"({nPlayers}/4) players")
        if i > 3: #4 == nPlayers
            handleJoinedGame()
            break
        else:
            i=i+1
        time.sleep(1)
        


def game_loop():
    # Game loop
    while True:
        # Clear terminal
        clear_terminal()
        
        # Update game state
        #TODO: con. function to get the players_map object
        #TODO: con. function to get the client_hand array
        #TODO: con. function to get the current_card string
        #TODO: con. function to get the current_card string
        #TODO: con. function to get the current_card string

        # Check if it's the player's turn
        if turn == client_name:
            printOptions()
            printUI(players_map,client_name,client_hand,current_card,turn,True)          
            player_choice = input("Enter your choice: ")
            # Process player choice on the server
            # TODO: Implement code here..
            # Update game state for the 3 globle variables below
            handlePlayerChoice(player_choice, client_hand, client_name)
            
        else:
            printUI(players_map,client_name,client_hand,current_card,turn,False)          
            # Wait for game state change

        # lil pause maybe send to the server end turn idk
        time.sleep(2)


if __name__ == "__main__":
    #Play again loop
    while(True):
        handerGameInit()

        #init server connection
        ip, port = handleGetServer()
        connection = Connection(ip, port)

        # print welcome message from server
        client_name = handleGetName()
        client_name = 'name = "' + client_name + '"'
        connection.send(client_name)

        
        lobby_loop(connection)
        game_loop(connection)
        #Play again?
        if ("y" == input("play again?")):
            pass
        else:
            break
    print("Done.")
