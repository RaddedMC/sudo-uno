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
    handlePlayerChoice2,
    handlePlayerChoice3,
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
import colorama


def clear_terminal():
    """Clears the terminal screen."""
    os.system("cls" if os.name == "nt" else "clear")


# Player and game state
players_map = {}

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


def lobby_loop(connection, client_name):
    clear_terminal()  # Lobby Loop (checks server every iteration to see if it should start the game)
    print(f"Welcome {client_name}!")

    while True:
        data = connection.receive()

        lobby_end(data)

        if "lobby.request.approved" in data:
            clear_terminal()
            print(f"Welcome {client_name}!")
            print("A player has joined the game!")
            lines = [line.strip() for line in data.split("\n")]
            players_index = lines.index("players=")
            players = lines[players_index + 1 :]
            for player in players:
                if player:
                    print(player.strip("“”"))

        # Check if the game has started
        if "lobby.start" in data:
            print("The game has started!")

            game_loop(connection, client_name)
            #TODO: maybe debug the wating room animation 
        #     player_names = [
        #         line.split("= ")[1].strip("“”")
        #         for line in data.split("\n")
        #         if "name" in line
        #     ]
        #     if client_name in player_names and len(player_names) == 4:
        #         handleJoinedGame()
        #         break

        # else:
        #     waitingForLobby(dots)
        #     dots += 1
        #     if dots > 3:
        #         dots = 1


def game_loop(connection, client_name):
    clear_terminal()
    turn = None

    # print in green
    print(colorama.Fore.GREEN + "The game has started!")
    print(colorama.Fore.RESET)

    # Game loop
    while True:
        # Clear terminal
        # Update game state
        data = connection.receive()

        lobby_end(data)

        if "game.state.update" in data:
            lines = data.split("\n")
            for line in lines:
                if "turn =" in line:
                    turn = line.split("= ")[1].replace('"', "")

                if "players =" in line:
                    players_index = lines.index(line)
                    players = lines[players_index + 1 : players_index + 5]
                    # remove /t from the beginning of the line
                    players = [player.strip() for player in players]

                    for player in players:
                        player = player.replace('"', "")
                        # add them to the players_map     "Player 3": 4,
                        player_name = player.split("|")[0]
                        player_cards = int(player.split("|")[1])
                        players_map[player_name] = player_cards

                if "current_card =" in line:
                    current_card = line.split("= ")[1].replace('"', "")

            if "your_cards =" in data:
                lines = [line.strip() for line in data.split("\n")]
                cards_index = lines.index("your_cards =")
                client_hand = [
                    card.replace('"', "")
                    for card in lines[cards_index + 1 :]
                    if card and "Your turn" not in card
                ]

        # Check if it's the player's turn
        if "Your turn" in data:
            printUI(players_map, client_name, client_hand, current_card, turn, True)
            player_choice = input("Enter your choice: ")

            if player_choice == "2":  # Assume "2" means pick up a card
                data = connection.send(
                    ["Turn.take", 'card = ""', "pick = true", "sudo = false"]
                )
                #TODO add logic to handle mapping the drawn card: 
                #--------------------------------------
                drawn_card = "Red|5"
                #--------------------------------------

                #Update ui to show what card got drawn
                handlePlayerChoice2(drawn_card)
            elif player_choice == "1":  # Assume "1" means place down a card
                #Get card to play from array given number returns card string
                #TODO: note if the card is not in hand will return "null" add logic server side to handle edge case
                card_to_play = handlePlayerChoice1(client_hand)
                data = connection.send(
                    [
                        "Turn.take\n",
                        f'card = "{card_to_play}"\n',
                        "pick = false\n",
                        "sudo = false",
                    ]
                )
            elif player_choice == "3": 
                if len(client_hand) == 2:
                    handlePlayerChoice3(client_name)
                    card_to_play = handlePlayerChoice1(client_hand)
                    #TODO: note if the card is not in hand will return "null" add logic server side to handle edge case
                    data = connection.send(
                        [
                            "Turn.take\n",
                            f'card = "{card_to_play}"\n',
                            "pick = false\n",
                            "sudo = true",
                        ]
                    )
                else:
                    print("You can only call sudo when you have 2 cards left. Please try again.")
                    pass
            
            lobby_end(data)

            # Handle server response
            if "Turn.approve" in data:
                print("Your turn was approved.")
                if "You forgot to say sudo." in data:
                    print("You forgot to say sudo draw 2.")
                #time.sleep(2) # Wait for 2 seconds then clear the display
                clear_terminal()

            elif "Turn.reject" in data:
                reason = data.split("reason: ")[1]
                print(f"Your turn was rejected. Reason: {reason}")
                #time.sleep(2) # Wait for 2 seconds then clear the display
                clear_terminal()


        else:
            printUI(players_map, client_name, client_hand, current_card, turn, False)
            # Wait for game state change


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
    handerGameInit()

    # init server connection
    # ip, port = handleGetServer()
    connection = Connection("127.0.0.1", "6969")

    #print welcome message from server

    # get the username as argument from the command line
    if len(sys.argv) < 2:
        client_name = handleGetName()
    else:
        client_name = sys.argv[1]

    response = connection.send(["Lobby.request", f'\nname = "{client_name}"'])

    lobby_end(response)

    client_name = response.split('\n\tname="')[1].split('"\n')[0]

    lobby_loop(connection, client_name)
    game_loop(connection)


if __name__ == "__main__":
    main()
