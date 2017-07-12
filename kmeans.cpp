#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cassert>
using namespace std;

class Point
{
public:
    double x, y;
    Point(){}
    Point(double x, double y)
    {
        this->x = x;
        this->y = y;
    }
    Point operator+ (const Point &p)
    {
        return Point(x + p.x, y + p.y);
    }
    Point operator- (const Point &p)
    {
        return Point(x - p.x, y - p.y);
    }
    Point operator/ (double number)
    {
        double epsilon = 1e-16;
        assert(fabs(number) > epsilon);
        return Point(x/number, y/number);
    }
    double norm() const
    {
        return sqrt(x*x + y*y);
    }
    double normSquared() const
    {
        return x*x + y*y;
    }
    string toString() const
    {
        return to_string(x) + "  " + to_string(y);
    }
    void print() const
    {
        cout << "(" << x << ", " << y << ")" << endl;
    }
};


double uniform()
{
    return rand()/float(RAND_MAX);
}

double uniform(double a, double b)
{
    return a + (b-a)*uniform();
}

double square(double a)
{
    return a*a;
}

Point getCentroid(vector<Point> points)
{
    assert(points.size() > 0);
    Point result(0,0);
    for (int i = 0; i < points.size(); ++i)
    {
        result = result + points[i];
    }
    result = result/float(points.size());
    return result;
}

class kmeans
{
private:
    int N;
    int k;
public:
    vector<Point> points;
    vector<vector<Point> > clusters;
    vector<Point> centroids;
    kmeans(){}
    kmeans(int N, int k)
    {
        this->N = N;
        this->k = k;
        assert(N >= k);
        for (int i = 0;  i < N/k; ++i)
        {
            double a = -0.2;
            double b = 0.2;
            points.push_back(Point(uniform(a, b), uniform(a, b)));
            points.push_back(Point(uniform(a, b), 1 + uniform(a, b)));
            points.push_back(Point(uniform(a, b) + 1, uniform(a, b) + 1));
            points.push_back(Point(uniform(a,b) + 1, uniform(a, b)));
        }
        for (int i = 0; i < k; ++i)
        {
            centroids.push_back(points[i]);
        }
        for (int i = 0; i < k; ++i)
        {
            vector<Point> temp;
            temp.push_back(centroids[i]);
            clusters.push_back(temp);
        }
    }
    void printPoints() const
    {
        for (int i = 0; i < points.size(); ++i)
        {
            cout << points[i].toString() << endl;
        }
    }
    void printPointsToFile(string fileName) const
    {
        ofstream ofile(fileName.c_str());
        for (int i = 0; i < points.size(); ++i)
        {
            ofile << points[i].toString() << endl;
        }
        ofile.close();
    }
    double wcss(int clusterIndex, Point p)
    {
        assert(clusters[clusterIndex].size() > 0);
        double sum = 0;
        clusters[clusterIndex].push_back(p);
        for (int i = 0; i < clusters[clusterIndex].size(); ++i)
        {
            sum = sum + (clusters[clusterIndex][i] - centroids[clusterIndex]).normSquared();
        }
        sum = sum/float(clusters[clusterIndex].size());
        clusters[clusterIndex].erase(clusters[clusterIndex].end() - 1);
        return sum;
    }
    void calculateCentroids()
    {
        assert(centroids.size() == clusters.size());
        assert(centroids.size() > 0);
        for (int i = 0; i < centroids.size(); ++i)
        {
            centroids[i] = getCentroid(clusters[i]);
        }
    }
    void update()
    {
        for (int i = 0; i < N; ++i)
        {
            int clusterIndex = 0;
            double minResult;
            vector<double> variance;
            for (int j = 0; j < clusters.size(); ++j)
            {
                double result = wcss(j, points[i]);
                variance.push_back(result);
            }
            minResult = variance[0];
            for (int j = 1; j < variance.size(); ++j)
            {
                if (variance[j] < minResult)
                {
                    clusterIndex = j;
                    minResult = variance[j];
                }
            }
            clusters[clusterIndex].push_back(points[i]);
        }
        calculateCentroids();
    }
    void classify()
    {
        int iterationMax = 10;
        int count = 0;

        for (int i = 0; i < centroids.size(); ++i)
        {
            centroids[i] = points[i];
        }
        while(count < iterationMax)
        {
            vector<Point> oldCentroids(centroids);
            for (int i = 0; i < clusters.size(); ++i)
            {
                clusters[i].clear();
            }
            clusters.clear();
            for (int i = 0; i < centroids.size(); ++i)
            {
                vector<Point> temp;
                temp.push_back(centroids[i]);
                clusters.push_back(temp);
            }
            update();
            count++;
            cout << "Iteration number = " << count << endl;

            double s = 0;
            for (int i = 0; i < centroids.size(); ++i)
            {
                s = s + (oldCentroids[i] - centroids[i]).normSquared();
            }
            if (s < 1e-16) break;
        }
    }
    void printClusters() const
    {
        for (int i = 0; i < clusters.size(); ++i)
        {
            string fileName = "cluster_" + to_string(i+1) + ".txt";
            ofstream ofile(fileName);
            for (int j = 0; j < clusters[i].size(); ++j)
            {
                ofile << clusters[i][j].toString() << endl;
            }
            ofile.close();
        }
    }
};

int main()
{
    int N = 200;
    int k = 4;

    kmeans machine(N, k);
    machine.printPointsToFile("points.txt");
    machine.classify();
    machine.printClusters();
    return 0;
}
