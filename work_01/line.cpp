class Line {

    private:

    public:

        int id;
        int x1, y1;
        int x2, y2;

        float linear_coeficient = 0.0f;
        float angular_coeficient = 0.0f;

        Line() {}

        Line( int x1, int y1, int x2, int y2 ) {
            this->x1 = x1;
            this->y1 = y1;
            this->x2 = x2;
            this->y2 = y2;
        }
};