import colorama
from colorama import Fore, Back, Style

colorama.init()

# Define color codes for card types
color_codes = {
    "Blue": Fore.BLUE,
    "Red": Fore.RED,
    "Green": Fore.GREEN,
    "Yellow": Fore.YELLOW,
    "Black": Fore.WHITE + Back.BLACK  # White text on black background for Black cards
}

type_char_map = {
    "0": "0",
    "1": "1",
    "2": "2",
    "3": "3",
    "4": "4",
    "5": "5",
    "6": "6",
    "7": "7",
    "8": "8",
    "9": "9",
    "Rev": "R",
    "Skip": "S",
    "PL2": "+2",
    "Wild": "W",
    "Wild4": "+4"
}

def print_card_type(card_type):
    type_map = {
        "0": r"""
  /▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔\
 | 0.------------.  |
 |  :    ____    :  |
 |  :  .'    '.  :  |
 |  : |  .--.  | :  |
 |  : | |    | | :  |
 |  : |  `--'  | :  |
 |  :  '.____.'  :  |
 |  :            :  |
 |  '------------'0 |
  \_________________/
""",
        "1": r"""

""",
        "2": r"""

""",
        "3": r"""

""",
        "4": r"""
 _  _  
| || | 
| || |_
|__   _|
   |_|
""",
        "5": r"""
 _____ 
| ____|
| |__  
|___ \ 
 ___) |
|____/ 
""",
        "6": r"""

""",
        "7": r"""

""",
        "8": r"""

""",
        "9": r"""

""",
        "Rev": r"""

""",
        "Skip": r"""

""",
        "PL2": r"""

""",
        "Wild": r"""

""",
        "Wild4": r"""

"""
    }

    if card_type in type_map:
        print(type_map[card_type].replace('#', '\n'))
    else:
        print(f"Invalid card type: {card_type}")
        


def printHand(hand):
    """Prints the player's hand of cards."""
    hand_str = " ".join([f"{color_codes.get(color, Fore.RESET)}{color}|{type}{Style.RESET_ALL}" for color, type in [card.split("|") for card in hand]])
    print(f"Your hand: {hand_str}")
    
def printHand2(hand):
    """Prints the player's hand of cards."""
    print("Your hand:")
    for i, card in enumerate(hand, start=1):
        color, type = card.split("|")
        color_code = color_codes.get(color, Fore.RESET)
        print(f"{i}. {color_code}{color}|{type}{Style.RESET_ALL}")

def printOpponentsHand(players_map, client_name):
    """Prints the opponents' names and the number of cards they have."""
    for player, num_cards in players_map.items():
        if player != client_name:
            print(f"{player} has {num_cards} cards: {' '.join(['?'] * num_cards)}")

def printTopcard(top_card):
    """Prints the top card on the discard pile."""
    color, type = top_card.split("|")
    color_code = color_codes.get(color, Fore.RESET)
    print(f"{Fore.YELLOW}{Style.BRIGHT}Top card:{Style.RESET_ALL} {color_code}{Style.BRIGHT}{color}|{type}{Style.RESET_ALL}")

# Updated printTurn function
def printTurn(player_name, is_current_turn):
    if is_current_turn:
        print(f"{Fore.GREEN}{Style.BRIGHT}It's your turn!{Style.RESET_ALL}")
    else:
        print(f"{Fore.RED}{Style.BRIGHT}Waiting for {player_name}'s turn...{Style.RESET_ALL}")

def printOptions(options):
    """Prints the available options for the player's turn."""
    print("Options:")
    for i, option in enumerate(options, start=1):
        print(f"{i}. {option}")
        
def handlePlayerChoice(choice, hand, client_name):
    if choice == "1":
        printHand2(hand)
        card_index = int(input("Enter the number corresponding to the card you want to play: "))
        if 1 <= card_index <= len(hand):
            played_card = hand[card_index - 1]
            color, type = played_card.split("|")
            color_code = color_codes.get(color, Fore.RESET)
            print(f"You played {color_code}{color}|{type}{Style.RESET_ALL}")
            # Add logic to update game state with the played card
        else:
            print("Invalid card number. Please try again.")
        
        # Add logic to update game state with the played card
    elif choice == "2":
        drawn_card = "Red|5"  # Replace with actual drawn card
        color, type = drawn_card.split("|")
        color_code = color_codes.get(color, Fore.RESET)
        print(f"You drew {color_code}{color}|{type}{Style.RESET_ALL}")
        # Add logic to update game state with the drawn card
    elif choice == "3":
        print(f"{Fore.RED}{Style.BRIGHT}{client_name} called SUDO!{Style.RESET_ALL}")
        # Add logic to handle UNO call
    else:
        print("Invalid choice. Please try again.")
        
def handerGameInit():
    #Add logo 
    print(f"{Fore.RED}{Style.BRIGHT} .----------------.  .----------------.  .----------------.  .----------------. \n| .--------------. || .--------------. || .--------------. || .--------------. |\n| |    _______   | || | _____  _____ | || |  ________    | || |     ____     | |\n| |   /  ___  |  | || ||_   _||_   _|| || | |_   ___ `.  | || |   .'    `.   | |\n| |  |  (__ \\_|  | || |  | |    | |  | || |   | |   `. \\ | || |  /  .--.  \\  | |\n| |   '.___`-.   | || |  | '    ' |  | || |   | |    | | | || |  | |    | |  | |\n| |  |`\\____) |  | || |   \\ `--' /   | || |  _| |___.' / | || |  \\  `--'  /  | |\n| |  |_______.'  | || |    `.__.'    | || | |________.'  | || |   `.____.'   | |\n| |              | || |              | || |              | || |              | |\n| '--------------' || '--------------' || '--------------' || '--------------' |\n '----------------'  '----------------'  '----------------'  '----------------' \n\n                                     ______                                     \n                                    |______|                                    \n\n           .----------------.  .-----------------. .----------------.           \n          | .--------------. || .--------------. || .--------------. |          \n          | | _____  _____ | || | ____  _____  | || |     ____     | |          \n          | ||_   _||_   _|| || ||_   \\|_   _| | || |   .'    `.   | |          \n          | |  | |    | |  | || |  |   \\ | |   | || |  /  .--.  \\  | |          \n          | |  | '    ' |  | || |  | |\\ \\| |   | || |  | |    | |  | |          \n          | |   \\ `--' /   | || | _| |_\\   |_  | || |  \\  `--'  /  | |          \n          | |    `.__.'    | || ||_____|\\____| | || |   `.____.'   | |          \n          | |              | || |              | || |              | |          \n          | '--------------' || '--------------' || '--------------' |          \n           '----------------'  '----------------'  '----------------'           {Style.RESET_ALL}")
    #Add game rules
    print(f"{Fore.YELLOW}{Style.BRIGHT}Game Rules:{Style.RESET_ALL}")
    #print_card_type("0")
    pass
    

def handleGetName():
    print(f"{Fore.WHITE}{Style.BRIGHT}Enter your username to get started:{Style.RESET_ALL}")
    return input("")

def handleGetServer():
    print(f"{Fore.WHITE}{Style.BRIGHT}Enter the server IP address:{Style.RESET_ALL}")
    ip = input("")
    print(f"{Fore.WHITE}{Style.BRIGHT}Enter the server port number:{Style.RESET_ALL}")
    port = input("")

    return ip, port

def waitingForLobby():
    print(f"{Fore.YELLOW}{Style.BRIGHT}Waiting for other players...{Style.RESET_ALL}")

def handleJoinedGame():
    #Print this in red font
    print(f"{Fore.GREEN}{Style.BRIGHT}Game has started!{Style.RESET_ALL}")


def handleGetNumPlayers():
    return int(input("Enter the number of players (2-4): "))

# Example usage
if __name__ == "__main__":
    printOpponentsHand("Player 2", 5)
    printHand(["Blue|Rev", "Red|Skip", "Green|PL2", "Black|Wild", "Black|Wild4", "Red|9"])
    printTopcard("Yellow|9")
    printTurn("Player 1")
    printOptions(["Play a card", "Draw a card", "Call UNO"])
    