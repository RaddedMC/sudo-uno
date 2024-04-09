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


import logging
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


def lobby_loop(connection):
    dots = 1
    # Lobby Loop (checks server every iteration to see if it should start the game)
    print(f"Welcome {client_name}!")

    while True:
        clear_terminal()
        print(f"Welcome {client_name}!")

        data = connection.receive()

        # Check if the game has started
        if "Lobby.start" in data:
            player_names = [
                line.split("= ")[1].strip("“”")
                for line in data.split("\n")
                if "name" in line
            ]
            if client_name in player_names and len(player_names) == 4:
                handleJoinedGame()
                break
        else:
            waitingForLobby(dots)
            dots += 1
            if dots > 3:
                dots = 1
        time.sleep(1)


def game_loop(connection):
    # Game loop
    while True:
        # Clear terminal
        clear_terminal()

        # Update game state
        data = connection.receive()
        if "Game.state.update" in data:
            for line in data.split("\n"):
                if "players" in line:
                    players_map = {
                        player.split("|")[0].strip("“”"): int(player.split("|")[1])
                        for player in line.split("\n")[1:]
                    }
                elif "your_cards" in line:
                    client_hand = [card.strip("“”") for card in line.split("\n")[1:]]
                elif "current_card" in line:
                    current_card = line.split(" = ")[1].strip("“”")
                elif "turn" in line:
                    turn = line.split(" = ")[1].strip("“”")

        # Check if it's the player's turn
        if turn == client_name:
            printOptions()
            printUI(players_map, client_name, client_hand, current_card, turn, True)
            player_choice = input("Enter your choice: ")

            if player_choice == "1":  # Assume "1" means pick up a card
                data = connection.send(
                    ["Turn.take", 'card = ""', "pick = true", "sudo = false"]
                )
            elif player_choice == "2":  # Assume "2" means place down a card
                card_to_play = input(
                    "Enter the card to play: "
                )  # Get the card to play from the user
                data = connection.send(
                    [
                        "Turn.take",
                        f'card = "{card_to_play}"',
                        "pick = false",
                        "sudo = false",
                    ]
                )

            # Handle server response
            if "Turn.approve" in data:
                print("Your turn was approved.")
                if "You forgot to say sudo." in data:
                    print("You forgot to say sudo.")
            elif "Turn.reject" in data:
                reason = data.split("reason: ")[1]
                print(f"Your turn was rejected. Reason: {reason}")

            # Update game state for the 3 global variables below
            handlePlayerChoice(player_choice, client_hand, client_name)

        else:
            printUI(players_map, client_name, client_hand, current_card, turn, False)
            # Wait for game state change

        # lil pause maybe send to the server end turn idk
        time.sleep(2)


def lobby_end(data):
    if "Lobby.end" in data:
        if "ragequit" in data:
            name = data.split("player ")[1].split(" ragequit")[0]
            print(f"The game ended because player {name} ragequit.")
        elif "placed their last card and won" in data:
            name = data.split("player ")[1].split(" placed their last card and won")[0]
            print(
                f"The game ended because player {name} placed their last card and won."
            )
        elif "unexplained error on the server" in data:
            print(
                "The game ended because there was an unexplained error on the server."
            )

        # Play again?
        if "y" == input("play again?"):
            main()
        else:
            # end the game
            print("Done.")
            sys.exit()


def main():
    while True:
        handerGameInit()

        # init server connection
        # ip, port = handleGetServer()
        connection = Connection("127.0.0.1", "6969")

        # print welcome message from server
        client_name = handleGetName()
        response = connection.send(["Lobby.request", f'name = "{client_name}"'])
        client_name = response.split('\n\tname="')[1].split('"\n')[0]

        lobby_loop(connection)
        game_loop(connection)
        # Play again?
        if "y" == input("play again?"):
            pass
        else:
            break
    print("Done.")


if __name__ == "__main__":
    main()
