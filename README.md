# Jetpack-Joyride
This is a pure C++ game application that uses SFML for the graphics and user interaction. It uses **Quad Tree** for efficient collision detection by spatially partitioning game objects, a **queue** to manage power-ups and their activation sequence, and a **BST** to maintain the leaderboard efficiently.

### Key Features  
- **Efficient Collision Detection:**  
  Utilizes a **Quad Tree** to optimize collision detection by spatially partitioning game objects.  
- **Power-Up Management:**  
  Implements a **queue** to handle power-ups and their activation sequence seamlessly.  
- **Leaderboard System:**  
  Uses a **Binary Search Tree (BST)** to maintain a sorted and efficient leaderboard for players.  
- **OOP Principles:**  
  - Separate classes for handling enemy and player logic.  
  - Inheritance is used in the power-up hierarchy for efficient management of different types of power-ups.

## Technologies Used  
- **C++**: Core programming language for game logic.  
- **SFML**: For graphics and user interaction.  

## How to Run  

1. Clone the repository:  
   `git clone <repository_url>`

2. Ensure you have SFML installed on your system.

3. Compile the project:  
   `g++ main.cpp -o game -lsfml-graphics -lsfml-window -lsfml-system`

4. Run the Game:  
   `./game.exe`



