#include <Zumo32U4.h>

Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4ButtonA buttonA;
Zumo32U4Buzzer buzzer;

unsigned int lineSensorValues[3];

struct Node {
  int x;
  int y;
  Node* neighbors[4];
  bool visited;
};

Node graph[100];
Node* path[100];
int pathLength = 0;
Node* source;
Node* destination;

int t1 = 600; // Threshold for detecting a line (black space)
int t2 = 250; // Threshold for detecting white space

Node* houses[5];

void setup() {
  buttonA.waitForPress();
  lineSensors.initThreeSensors();
  proxSensors.initThreeSensors();
  buttonA.waitForRelease();

  // Define the coordinates of houses
  houses[0] = &graph[0];  // House 1
  houses[1] = &graph[20]; // House 2
  houses[2] = &graph[42]; // House 3
  houses[3] = &graph[24]; // House 4
  houses[4] = &graph[2];  // House 5
}

void loop() {
  // Explore the maze and build the graph
  exploreMaze();

  // Get the house number from the argument
  int houseNumber = 1; // change to go to a specefic house

  if (houseNumber >= 1 && houseNumber <= 5) {
    // Set the destination house based on the house number
    destination = houses[houseNumber - 1];

    // Find the shortest path from source to destination
    findShortestPath();

    // Navigate the shortest path
    navigateShortestPath();
  } else {
      // do not do anything
  }

}


void exploreMaze() {
  // Initialize the source node
  source = &graph[0];
  source->visited = false;

  // Start exploring from  source node
  exploreNode(source);
}

void exploreNode(Node* node) {
  node->visited = true;

  // Explore  neighboring nodes
  if (canMoveForward()) {
    Node* neighbor = &graph[node->y * 10 + node->x + 1];
    addNeighbor(node, neighbor);
    moveForward();
    exploreNode(neighbor);
    moveBackward();
  }

  if (canTurnLeft()) {
    Node* neighbor = &graph[node->y * 10 + node->x - 10];
    addNeighbor(node, neighbor);
    turnLeft();
    moveForward();
    exploreNode(neighbor);
    moveBackward();
    turnRight();
  }

  if (canTurnRight()) {
    Node* neighbor = &graph[node->y * 10 + node->x + 10];
    addNeighbor(node, neighbor);
    turnRight();
    moveForward();
    exploreNode(neighbor);
    moveBackward();
    turnLeft();
  }
}

void addNeighbor(Node* node, Node* neighbor) {
  for (int i = 0; i < 4; i++) {
    if (node->neighbors[i] == nullptr) {
      node->neighbors[i] = neighbor;
      break;
    }
  }
}

void findShortestPath() {
  // Perform BFS to find the shortest path from source to destination
  Node* queue[100];
  int front = 0, rear = 0;
  Node* parent[100] = {nullptr};
  queue[rear++] = source;

  while (front < rear) {
    Node* current = queue[front++];

    if (current == destination) {
      break;
    }

    for (int i = 0; i < 4; i++) {
      Node* neighbor = current->neighbors[i];
      if (neighbor != nullptr && !neighbor->visited) {
        neighbor->visited = true;
        queue[rear++] = neighbor;
        parent[neighbor->y * 10 + neighbor->x] = current;
      }
    }
  }

  // Construct the shortest path
  Node* current = destination;
  while (current != source) {
    path[pathLength++] = current;
    current = parent[current->y * 10 + current->x];
  }
  path[pathLength++] = source;

  // Reverse the path
  for (int i = 0; i < pathLength / 2; i++) {
    Node* temp = path[i];
    path[i] = path[pathLength - i - 1];
    path[pathLength - i - 1] = temp;
  }
}

void navigateShortestPath() {
  for (int i = 1; i < pathLength; i++) {
    Node* current = path[i];
    Node* prev = path[i - 1];

    if (current->x > prev->x) {
      turnRight();
    } else if (current->x < prev->x) {
      turnLeft();
    }

    moveForward();
  }
}

bool canMoveForward() {
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
  return lineSensorValues[1] < t2; // Check if the middle sensor detects white space
}

bool canTurnLeft() {
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
  return lineSensorValues[0] < t2; // Check if the left sensor detects white space
}

bool canTurnRight() {
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
  return lineSensorValues[2] < t2; // Check if the right sensor detects white space
}

void moveForward() {
  motors.setSpeeds(100, 100);
  delay(500);
  motors.setSpeeds(0, 0);
}

void moveBackward() {
  motors.setSpeeds(-100, -100);
  delay(500);
  motors.setSpeeds(0, 0);
}

void turnLeft() {
  motors.setSpeeds(-200, 200);
  delay(250);
  motors.setSpeeds(0, 0);
}

void turnRight() {
  motors.setSpeeds(200, -200);
  delay(250);
  motors.setSpeeds(0, 0);
}
