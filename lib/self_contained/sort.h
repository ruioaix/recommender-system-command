#ifndef CN_SORT_H
#define CN_SORT_H

void qsort_i_desc(int s[], int l, int r);
void qsort_i_asc(int s[], int l, int r);
void qsort_ii_asc(int s[], int l, int r, int in[]);
void qsort_ii_desc(int s[], int l, int r, int in[]);
void qsort_i3_asc(int s[], int l, int r, int in[], int in2[]);
void qsort_id3_asc(int s[], int l, int r, double in[], double in1[], double in2[]);
void qsort_d_asc(double s[], int l, int r);
void qsort_d_desc(double s[], int l, int r);
void qsort_di_desc(double s[], int l, int r, int in[]);
void qsort_di_asc(double s[], int l, int r, int in[]);
void qsort_iid_asc(int s[], int l, int r, int in[], double in1[]);

void qsort_li_desc(long s[], int l, int r, int in[]);

//int binsearch_i_asc(int source[], int length, int key);

#endif
