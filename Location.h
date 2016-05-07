#ifndef LOCATION_H_
#define LOCATION_H_

/*
    struct for representing locations in the grid.
*/
struct Location
{
    int row, col;

    Location()
    {
        row = col = 0;
    };

    Location(int r, int c)
    {
        row = r;
        col = c;
    };

    friend bool operator== (Location &cP1, Location &cP2)
    {
        return (cP1.col == cP2.col && cP1.row == cP2.row);
    };

    friend bool operator!= (Location &cP1, Location &cP2)
    {
        return !(cP1 == cP2);
    };

    friend ostream& operator<<(ostream& os, const Location& loc)
    {
        os << "(" << loc.row << ", " << loc.col << ")" << dt.yr;
        return os;
    }
};

#endif //LOCATION_H_
