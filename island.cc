// Danesh Daroui, 2016-10-23

#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <climits>
#include <ctime>

#define USER_DEFINED_GRID 0

using namespace std;

class grid {
  private:
    vector<char> g;
    vector<char> g_backup;
    int m, n;

    void setElem(int x, int y, int val, vector<char> &v) {
      v[y * m + x] = val;
    }

    char getElem(int x, int y, vector<char> &v) {
      return v[y * m + x];
    }

    // Return "true" if a coordinate is in the range of the grid, otherwise
    // return "false".
    bool isInRange(int x, int y);

    // Check whether a '1' in the grid is an isolated island i.e. is not
    // connected to any node in vertical nor horizontal direction.
    bool isIsolated(int x, int y, vector<char> &v);

    // Destroy an island connected to the cell at coordonate (x, y), by making
    // all its elements to zero.
    void destroyIsland(int x, int y, vector<char> &v);

    bool solveMazeUtil(int x, int y, int *steps, vector<pair<int, int> > dirs);

  public:
    grid(int m, int n) {
      this->m = m;
      this->n = n;
      g.resize(m * n);
    }

    virtual ~grid() {
      g.clear();
      g_backup.clear();
    }

    // Dump all values inside a grid. If the "isol" flag is set to "true", then
    // the isolation status of each cell will be printed istead of its value.
    // Mostly used for debugging purposes.
    void dumpGrid(bool isol) {
      cout << endl;

      for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
          if (isol) {
            cout << isIsolated(j, i, g) << "  ";
          } else {
            char e = getElem(j, i, g);
            string s = "";

            if (e == '+') {
              s =  "\033[0;32m+\033[0m";
            } else {
              s = e;
            }

            cout << s << "  ";
          }
        }

        cout << endl;
      }

      cout << endl;
    }

    // Set all cells inside the grid to '0' which will make it clear.
    void clearGrid() {
      for (vector<char>::iterator it = g.begin(); it != g.end(); ++it) {
        *it = '0';
      }
    }

    // Set all coordinates given by pairs to '1' in the grid. Note that this
    // function will not clear the grid as it should be called explicitely.
    void setGrid(vector<pair<int, int> > &v) {
      for (vector<pair<int, int> >::iterator it = v.begin();
           it != v.end();
           ++it) {
        setElem(it->first, it->second, '1', g);
      }
    }

    // Save the current state of the grid.
    void saveGrid() {
      g_backup.clear();
      g_backup.resize(m * n);
      copy(g.begin(), g.end(), g_backup.begin());
    }

    // Restore the previous saved grid.
    void restoreGrid() {
      g.clear();
      copy(g_backup.begin(), g_backup.end(), g.begin());
    }

    // Fill a random grid with the given dimensions.
    void randGrid() {
      clearGrid();

      srand(time(NULL));

      int nn = rand() % (m * n) + 1;

      for (int i = 0; i < nn; ++i) {
        int x = rand() % n;
        int y = rand() % m;

        setElem(x, y, '1', g);
      }
    }

    // Set the cell at (i, j) to '1' and then return the number of islands in
    // the grid.
    int updateAndCount(int x, int y);

    // Solve the maze by marking the path from upper-left to lower-right cells
    // and return number of steps to traverse the path.
    int solveMaze();

    // Solve the maze by finding the best path from upper-left to lower-right
    // cells. Note that the best path is not necessarily the shortest path.
    int solveMazeBest();
};

bool grid::isInRange(int x, int y) {
  return ((x < n) && (x >= 0) && (y < m) && (y >= 0));
}

// Return true if an element is an isolated single island means that it is not
// connected vertically nor horizoantally to any '1'. The function will return
// correct results only for binary grids i.e. grids consisting of zeros and ones
// and not the grid filled with arbitrary integers.
bool grid::isIsolated(int x, int y, vector<char> &v) {
  if ((getElem(x, y, v) == '0') || !isInRange(x, y)) {
    return false;
  }

  // Check if right horizontal neighbour exists.
  if ((x + 1) < n) {
    if (getElem(x + 1, y, v) == '1') {
      return false;
    }
  }

  // Check if left horizontal neighbour exists.
  if ((x - 1) >= 0) {
    if (getElem(x - 1, y, v) == '1') {
      return false;
    }
  }

  // Check if up vectical neighbour exists.
  if ((y + 1) < m) {
    if (getElem(x, y + 1, v) == '1') {
      return false;
    }
  }

  // Check if up vectical neighbour exists.
  if ((y - 1) >= 0) {
    if (getElem(x, y - 1, v) == '1') {
      return false;
    }
  }

  return true;
}

void grid::destroyIsland(int x, int y, vector<char> &v) {
  if ((getElem(x, y, v) == '0') || !isInRange(x, y)) {
    return;
  }

  if (isIsolated(x, y, v)) {
    setElem(x, y, '0', v);
    return;
  }

  // Set the visited element to '0' to mark it as visited.
  setElem(x, y, '0', v);

  // Now destroy all cells in this island since this island has already been
  // counted once.
  destroyIsland(x + 1, y, v);
  destroyIsland(x - 1, y, v);
  destroyIsland(x, y + 1, v);
  destroyIsland(x, y - 1, v);

  return;
}

// Set the cell (x, y) to one and return number of islands in the grid.
int grid::updateAndCount(int x, int y) {
  int num_of_islands = 0;

  if (!isInRange(x, y)) {
    // Either 'x' or 'y' is out of range.
    return 0;
  }

  // Mark the cell at the given coordinate to '1'.
  setElem(x, y, '1', g);

  // Copy the original vector, so destroying islands will not affect the main
  // grid and the original order will be preserved.
  vector<char> v(g);

  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      if (getElem(j, i, v) == '1') {
        // When a cell is '1' it means that there is definitely an island. So
        // the number of islands should be icreased and then the island should
        // be destroyed.
        ++num_of_islands;
        destroyIsland(j, i, v);
      }
    }
  }

  return num_of_islands;
}

bool grid::solveMazeUtil(int x,
                         int y,
                         int *steps,
                         vector<pair<int, int> > dirs) {
  if ((x == n - 1) && (y == m - 1)) {
    // The destination has been reached, so mark the cell and return "true".
    setElem(x, y, '+', g);
    ++(*steps);

    return true;
  }

  if ((getElem(x, y, g) == '0') && isInRange(x, y)) {
    // The current cell is not a wall and it is within the range of the grid. So
    // mark it as the path and try to solve the path through one of the east,
    // south, north, or west directions.
    setElem(x, y, '+', g);
    ++(*steps);

    for (vector<pair<int, int> >::iterator it = dirs.begin();
         it != dirs.end();
         ++it) {
      if (solveMazeUtil(x + it->first, y + it->second, steps, dirs) == true) {
        return true;
      }
    }

    // There is no path from this solution so just unmark it and perform a
    // backtrack.
    setElem(x, y, '0', g);
    --(*steps);
    return false;
  }

  // Current cell is either a wall or it is out of range of the grid.
  return false;
}

int grid::solveMaze() {
  int steps = 0;
  vector<pair<int, int> > dirs;

  // The start and end the end points in the maze shoud always bee free.
  if ((getElem(0, 0, g) == '1') || (getElem(n - 1, m - 1, g) == '1')) {
    return 0;
  }

  dirs.push_back(make_pair(1, 0));
  dirs.push_back(make_pair(0, 1));
  dirs.push_back(make_pair(0, -1));
  dirs.push_back(make_pair(-1, 0));

  if (solveMazeUtil(0, 0, &steps, dirs)) {
    // A solution has been found and the number of steps will be sent back while
    // the grid is also updated with the found path marked in the grid.
    return steps;
  } else {
    // No solution has been found in the grid.
    return 0;
  }
}

int grid::solveMazeBest() {
  int min_path = INT_MAX;
  vector<pair<int, int> > dirs;
  vector<int> inds;
  vector<char> v(g);              // Save the original grid.
  vector<char> best(g);           // Best found solution in the grid.

  // The start and end the end points in the maze shoud always bee free.
  if ((getElem(0, 0, g) == '1') || (getElem(n - 1, m - 1, g) == '1')) {
    return 0;
  }

  dirs.clear();
  dirs.push_back(make_pair(1, 0));
  dirs.push_back(make_pair(0, 1));
  dirs.push_back(make_pair(0, -1));
  dirs.push_back(make_pair(-1, 0));

  inds.clear();
  inds.push_back(0);
  inds.push_back(1);
  inds.push_back(2);
  inds.push_back(3);

  bool solution_found = false;

  // Try all solutions with all permutations of priorities in directions.
  do {
    vector<pair<int, int> > tmp_dirs;

    tmp_dirs.push_back(dirs[inds[0]]);
    tmp_dirs.push_back(dirs[inds[1]]);
    tmp_dirs.push_back(dirs[inds[2]]);
    tmp_dirs.push_back(dirs[inds[3]]);
    
    int steps = 0;
    if (solveMazeUtil(0, 0, &steps, tmp_dirs) && (steps < min_path)) {
      // A good solution has been founs, so set the flag, store the path length
      // and store the path and the grid.
      min_path = steps;
      solution_found = true;
      copy(g.begin(), g.end(), best.begin());
    }

    // Restore the grid for next turn.
    copy(v.begin(), v.end(), g.begin());
  } while (next_permutation(inds.begin(), inds.end()));


  if (!solution_found) {
    // Probably dead-end has happened.
    return -1;
  }

  // Return the path and the number steps as the solution.
  copy(best.begin(), best.end(), g.begin());
  return min_path;
}


int main() {
#if USER_DEFINED_GRID == 1
  vector<pair<int, int> > v;
  v.push_back(make_pair(0, 1));
  v.push_back(make_pair(1, 1));
  v.push_back(make_pair(1, 3));
  v.push_back(make_pair(1, 4));
  v.push_back(make_pair(1, 6));
  v.push_back(make_pair(1, 7));
  v.push_back(make_pair(1, 8));
  v.push_back(make_pair(1, 9));
  v.push_back(make_pair(2, 2));
  v.push_back(make_pair(4, 2));
  v.push_back(make_pair(5, 1));
  v.push_back(make_pair(3, 2));
  v.push_back(make_pair(3, 5));
  v.push_back(make_pair(3, 9));
  v.push_back(make_pair(4, 9));
  v.push_back(make_pair(5, 9));
  v.push_back(make_pair(7, 2));
  v.push_back(make_pair(7, 6));
  v.push_back(make_pair(7, 7));
  v.push_back(make_pair(7, 8));
  v.push_back(make_pair(9, 10));
  v.push_back(make_pair(11, 3));
  v.push_back(make_pair(10, 3));
  v.push_back(make_pair(9, 3));
  v.push_back(make_pair(8, 3));

  grid *gg = new grid(12, 12);

  cout << endl << "Filling the grid with '1's at arbitrary places" << endl;
  cout << "----------------------------------------------" << endl;

  gg->clearGrid();
  gg->setGrid(v);

  int n = gg->updateAndCount(1, 2);
  gg->dumpGrid(false);

  cout << "Number of islands in the above grid: " << n << endl << endl;

  int steps;

  steps = gg->solveMaze();

  if (steps > 0) {
    cout << "Found solution in the maze at " << steps << " steps." << endl;
    gg->dumpGrid(false);
  } else {
    cout << "The maze is a dead-end!" << endl;
  }

  gg->clearGrid();
  gg->setGrid(v);

  cout << "Thinking..." << flush;
  steps = gg->solveMazeBest();
  cout << " Done!" << endl;

  if (steps > 0) {
    cout << "Found best solution in the maze at " << steps << " steps." << endl;
    gg->dumpGrid(false);
  } else {
    cout << "The maze is a dead-end!" << endl;
  }

  delete gg;
#else
  grid *gg = new grid(12, 12);

  cout << endl << "Filling the grid with '1's at arbitrary places" << endl;
  cout << "----------------------------------------------" << endl;

  // Generate a random grid and save it for later use for finding the best path.
  gg->clearGrid();
  gg->randGrid();
  gg->saveGrid();

  int n = gg->updateAndCount(1, 2);
  gg->dumpGrid(false);

  cout << "Number of islands in the above grid: " << n << endl << endl;

  int steps = 0;
  // This flag will be set to true is the original was not solvable and new grid
  // has been generated.
  bool new_grid = false;

  cout << "--> Generating a solvable grid. Please wait..." << flush;
  while (true) {
    steps = gg->solveMaze();

    if (steps > 0) {
      break;
    }

    // The grid is not solvable. Generate another one!
    gg->clearGrid();
    gg->randGrid();
    gg->saveGrid();

    new_grid = true;
  }

  if (!new_grid) {
    cout << " Done!" << flush << endl << endl;
  } else {
    cout << " Done! (Genarated grid may differ from the original one)"
         << flush
         << endl
         << endl;
  }

  cout << "Found solution in the maze at " << steps << " steps." << endl;
  gg->dumpGrid(false);

  // Restore the original grid to find the best path.
  gg->restoreGrid();

  cout << "--> Thinking..." << flush;
  steps = gg->solveMazeBest();
  cout << " Done!" << flush << endl << endl;

  cout << "Found best solution in the maze at " << steps << " steps." << endl;
  gg->dumpGrid(false);

  delete gg;
#endif

  return 0;
}

