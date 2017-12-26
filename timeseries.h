#ifndef TIMESERIES_H
#define TIMESERIES_H

#include <cstdlib>
#include <QString>
#include <QList>
#include "measure.h"
#include "utility.h"

namespace awfm {
    class Timeseries
    {
    private:
        QList<Measure> data_;
        double errorCode_;
    public:
        Timeseries();

        void append(double t, double v);
        void append(double t);
        void append(Measure m);

        void readFromFile(QString file_path);
        void writeToFile(QString file_path);

        void setErrorCode(double errorCode);
        void setT(int idx, double t);
        void setV(int idx, double v);

        QList<Measure> data() { return data_; }
        double t(int idx) { return data_[idx].t(); }
        QList<double> ts();
        QList<double> vs();
        double v(int idx) { return data_[idx].v(); }
        double errorCode() { return errorCode_; }
        Measure at(int idx) { return data_[idx]; }
        size_t size() { return data_.size(); }

        /* Methods return a new Timeseries object when they extract
         * a subsection from the Timeseries. Otherwise, the object itself
         * is modified and returns void */
        void absolute();
        void averageByTolerance();
        void averageBySign();
        void consolidateZeros();
        Timeseries deltaValues();
        void dtdv();
        bool isEqual(Timeseries &ts, double tol=1e-6);
        double linearInterpolateValueAtTime(double t);
        void projectOntoLine(double tmin, double tmax, double dt);
        void scale(double s);
        int sign(int idx);
        void signChanges(QList<int> *idxs, QList<int> *signs);
        Timeseries slice(size_t start_idx, size_t end_idx);
        Timeseries sliceByTime(double tmin, double tmax);
        double valueMean();
        double valueMedian();
        void zeroBelow(double min_value);
    };
}
#endif // TIMESERIES_H
