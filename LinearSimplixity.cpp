//Linear Simplex Solver Made in C++17
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>    
#include <iomanip>
using namespace std;
struct SimplexSolver {
    vector<vector<double>> tableau;
    int numVars, numConstraints;
    bool isMaximization;

    SimplexSolver(int vars, int constraints, bool maximize)
        : numVars(vars), numConstraints(constraints), isMaximization(maximize) {
        tableau.resize(constraints + 1, vector<double>(vars + constraints + 2, 0));
    }

    void setObjective(vector<double>& coefficients) {
        for (int i = 0; i < numVars; i++) {
            tableau[numConstraints][i] = isMaximization ? -coefficients[i] : coefficients[i];
        }
    }

    void setConstraint(int row, vector<double>& coefficients, double rhs) {
        for (int i = 0; i < numVars; i++) {
            tableau[row][i] = coefficients[i];
        }
        tableau[row][numVars + row] = 1.0;
        tableau[row][numVars + numConstraints + 1] = rhs;
    }

    bool isOptimal() {
        for (int i = 0; i < numVars + numConstraints; i++) {
            if (tableau[numConstraints][i] < -1e-10) return false;
        }
        return true;
    }

    pair<int, int> findPivot() {
        int pivotCol = -1;
        double minVal = -1e-10;
        for (int i = 0; i < numVars + numConstraints; i++) {
            if (tableau[numConstraints][i] < minVal) {
                minVal = tableau[numConstraints][i];
                pivotCol = i;
            }
        }

        if (pivotCol == -1) return {-1, -1};

        int pivotRow = -1;
        double minRatio = 1e20;
        for (int i = 0; i < numConstraints; i++) {
            if (tableau[i][pivotCol] > 1e-10) {
                double ratio = tableau[i][numVars + numConstraints + 1] / tableau[i][pivotCol];
                if (ratio >= 0 && ratio < minRatio) {
                    minRatio = ratio;
                    pivotRow = i;
                }
            }
        }

        return {pivotRow, pivotCol};
    }

    void pivot(int row, int col) {
        double pivotElement = tableau[row][col];
        for (int j = 0; j < tableau[row].size(); j++) {
            tableau[row][j] /= pivotElement;
        }

        for (int i = 0; i < tableau.size(); i++) {
            if (i != row && abs(tableau[i][col]) > 1e-10) {
                double factor = tableau[i][col];
                for (int j = 0; j < tableau[i].size(); j++) {
                    tableau[i][j] -= factor * tableau[row][j];
                }
            }
        }
    }

    bool solve() {
        while (!isOptimal()) {
            auto [pivotRow, pivotCol] = findPivot();
            if (pivotRow == -1) {
                cout << "Unbounded solution\n";
                return false;
            }
            pivot(pivotRow, pivotCol);
        }
        return true;
    }

    void printSolution() {
        cout << "\n=== SOLUTION ===\n";
        cout << "Optimal Value: " << tableau[numConstraints][numVars + numConstraints + 1] << "\n";
        cout << "Variable Values:\n";
        for (int i = 0; i < numVars; i++) {
            double value = 0;
            for (int j = 0; j < numConstraints; j++) {
                if (abs(tableau[j][i] - 1.0) < 1e-10) {
                    bool isBasic = true;
                    for (int k = 0; k < numConstraints; k++) {
                        if (k != j && abs(tableau[k][i]) > 1e-10) {
                            isBasic = false;
                            break;
                        }
                    }
                    if (isBasic) {
                        value = tableau[j][numVars + numConstraints + 1];
                        break;
                    }
                }
            }
            cout << "  x" << i + 1 << " = " << fixed << setprecision(6) << value << "\n";
        }
    }
};

int main(){
    int numVars, numConstraints;

    cout << "=== LP SIMPLEX SOLVER ===\n";
    cout << "Enter number of variables: ";
    cin >> numVars;
    cout << "Enter number of constraints: ";
    cin >> numConstraints;

    int choice;
    cout << "Maximize (1) or Minimize (0)? ";
    cin >> choice;
    bool maximize = (choice == 1);
    SimplexSolver solver(numVars, numConstraints, maximize);
    cout << "\nEnter objective function coefficients:\n";
    vector<double> objective(numVars);
    for (int i = 0; i < numVars; i++) {
        cout << "  c" << i + 1 << " = ";
        cin >> objective[i];
    }
    solver.setObjective(objective);

    cout << "\nEnter constraint coefficients and RHS:\n";
    for (int i = 0; i < numConstraints; i++) {
        cout << "Constraint " << i + 1 << ":\n";
        vector<double> coeffs(numVars);
        for (int j = 0; j < numVars; j++) {
            cout << "  a" << i + 1 << j + 1 << " = ";
            cin >> coeffs[j];
        }
        double rhs;
        cout << "  RHS = ";
        cin >> rhs;
        solver.setConstraint(i, coeffs, rhs);
    }

    cout << "\nSolving...\n";
    if (solver.solve()) {
        solver.printSolution();
    }

    return 0;
}
