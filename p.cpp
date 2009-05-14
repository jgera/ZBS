#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <ostream>
#include <fstream>
#include "gvc.h"

using namespace std;

// - - - - Configuration - - - - - - - - -- - - - - - - - - - - - - - - 

bool frigor = false;
int NUM_RUNS =10;
bool feuc_dist = false;		// uses euclidean distances, the default is toroidal
bool fno_res = false;
bool fraw = false;

// - - - - End configuration  - - - - - - - - - - - - - - - - - - - - - 

#define CONNECTED(u,v)			(adjlist[(u)].find((v)) != adjlist[(u)].end())
#define EUC_DIST(xi,yi,xj,yj)	sqrt(((xi)-(xj))*((xi)-(xj)) + ((yi)-(yj))*((yi)-(yj)))

// E[C_i], for i=0 to 100, generated by a.cpp
/* polar-random {{{ */
const double kov[] = {0, 1,
1.289887,
1.576748,
1.748795, 
1.953086,
2.047847,
2.201017,
2.247586,
2.352883,
2.385542,
2.508473,
2.523975,
2.632168,
2.66575 ,
2.726101,
2.777126,
2.755609,
2.823536,
2.894328,
2.938573,
2.918329,
2.969449,
3.008761,
2.989566,
3.037981,
3.02805 ,
3.063032,
3.064815,
3.085028,
3.1414  ,
3.11479 ,
3.149835,
3.152637,
3.164064,
3.183672,
3.192712,
3.208818,
3.251408,
3.248416,
3.244883,
3.250835,
3.286136,
3.28534 ,
3.336174,
3.28534 ,
3.303515,
3.304184,
3.312301,
3.340376,
3.342572,
3.325638,
3.328025,
3.343654,
3.380165,
3.402319,
3.394457,
3.36514 ,
3.388409,
3.401237,
3.411836,
3.428006,
3.440675,
3.42199 ,
3.424728,
3.427274,
3.4239  ,
3.448028,
3.445768,
3.45866 ,
3.456941,
3.444909,
3.495202,
3.455095,
3.480114,
3.480559,
3.48613 ,
3.510958,
3.460251,
3.493101,
3.484443,
3.531043,
3.523659,
3.516879,
3.51465 ,
3.520635,
3.529388,
3.532348,
3.526905,
3.523022,
3.519393,
3.544794,
3.530184,
3.53741 ,
3.559246,
3.574206,
3.584137,
3.564912,
3.549856,
3.553293,
3.551765};
//}}}

/* XY random {{{
const double kov[] = {0, 1,
1.413497, 
1.73161 ,
1.980575,
2.178741,
2.339071,
2.470821,
2.580676,
2.673519,
2.752959,
2.8217  ,
2.881788,
2.934793,
2.981935,
3.02417 ,
3.062259,
3.096812,
3.128325,
3.157204,
3.183784,
3.208344,
3.231121,
3.252312,
3.272089,
3.290597,
3.307961,
3.324292,
3.339684,
3.354222,
3.367979,
3.381019,
3.393403,
3.40518 ,
3.416397,
3.427097,
3.437315,
3.447086,
3.456441,
3.465408,
3.474011,
3.482273,
3.490217,
3.49786 ,
3.505222,
3.512318,
3.519163,
3.525771,
3.532156,
3.538328,
3.5443  ,
3.550081,
3.555681,
3.561109,
3.566374,
3.571482,
3.576441,
3.581259,
3.585941,
3.590494,
3.594923,
3.599234,
3.603431,
3.607519,
3.611503,
3.615387,
3.619175,
3.62287 ,
3.626476,
3.629997,
3.633436,
3.636796,
3.640079,
3.643289,
3.646428,
3.649498,
3.652502,
3.655443,
3.658321,
3.66114 ,
3.663902,
3.666607,
3.669259,
3.671858,
3.674406,
3.676905,
3.679357,
3.681762,
3.684123,
3.68644 ,
3.688715,
3.690949,
3.693143,
3.695299,
3.697416,
3.699498,
3.701543,
3.703554,
3.705532,
3.707476,
3.709389};
}}} */

// Let X be the event that u is between r and 2r away from v
// Let Y be the event that u shares no neighbor with v
double r;								// radio range
int n;									// number of nodes
double *x, *y;
set<int> *adjlist;						// adjlist[i] stores r-neighbors of i
set<int> *adjlist2;						// adjlist2[i] stores node between r and 2r away
map< pair<int, int>, double > uvdist;	// stores neighboring nodes' distances

bool pair_found_in(int u, int v, set< pair<int, int> > const& s)
{ //{{{
	if (u < v)
		return s.find(pair<int, int>((u), (v))) != s.end();
	else
		return s.find(pair<int, int>((v), (u))) != s.end();
} //}}}

void save_pair_in(int u, int v, set< pair<int, int> >& s)
{ //{{{
	s.insert((u) < (v) ? pair<int, int>((u), (v)) : pair<int, int>((v), (u)));
} //}}}

/**
 * @param i					Node ID of the first node
 * @param j					Node ID of the second node
 * @param area_width 	Width of the simulation area
 * @param area_height	Height of the simulation area
 */
double distance(int i, int j, double area_width, double area_height)
{ //{{{
	double tmp;	

	// ensure order
	if (i > j) {
		tmp = i;
		i = j;
		j = (int)tmp;
	}

	// query database
	pair<int,int> p(i, j);
	map< pair<int,int>,double >::iterator iter = uvdist.find(p);
	if (iter != uvdist.end())
		return iter->second;
	
	if (feuc_dist) {
		tmp = EUC_DIST(x[i], y[i], x[j], y[j]);
		uvdist[p] = tmp;
		return tmp;
	} else {	
		double min;

		min = EUC_DIST(x[i],				y[i],				x[j], y[j]);
		tmp = EUC_DIST(x[i] + area_width,	y[i],				x[j], y[j]);
		if (tmp < min) min = tmp;
		tmp = EUC_DIST(x[i] - area_width,	y[i],				x[j], y[j]);
		if (tmp < min) min = tmp;
		tmp = EUC_DIST(x[i],				y[i] + area_height,	x[j], y[j]);
		if (tmp < min) min = tmp;
		tmp = EUC_DIST(x[i],				y[i] - area_height,	x[j], y[j]);
		if (tmp < min) min = tmp;
		tmp = EUC_DIST(x[i] + area_width,	y[i] + area_height,	x[j], y[j]);
		if (tmp < min) min = tmp;
		tmp = EUC_DIST(x[i] + area_width,	y[i] - area_height,	x[j], y[j]);
		if (tmp < min) min = tmp;
		tmp = EUC_DIST(x[i] - area_width,	y[i] + area_height,	x[j], y[j]);
		if (tmp < min) min = tmp;
		tmp = EUC_DIST(x[i] - area_width,	y[i] - area_height,	x[j], y[j]);
		if (tmp < min) min = tmp;
		uvdist[p] = min;
		return min;
	}
} //}}}

inline double joint_area(int u, int v)
{ //{{{
	double theta = 2*acos(distance(u,v,1,1)/2./r);
	return r*r*(theta-sin(theta));
} //}}}

template <typename T> void get_stat(T const& v, double &avg, double &stddev)
{ //{{{
	typename T::const_iterator iter;

	avg = stddev = 0;
	for (iter = v.begin(); iter != v.end(); iter++) {
		avg += (*iter);
		stddev += (*iter)*(*iter);
	}
	avg /= v.size();
	stddev = sqrt(stddev/v.size() - avg*avg);
} //}}}

GVC_t *gv_start(char *outfile)
{ //{{{
	GVC_t *gvc = gvContext();
	char *args[5];
	args[0] = "";
	args[1] = "-Tps2";
	args[2] = "-Kneato";
	args[3] = new char[3+strlen(outfile)];
	sprintf(args[3], "-o%s", outfile);
	args[4] = NULL;
	gvParseArgs(gvc, sizeof(args)/sizeof(char *)-1, args);
	//delete[] args[3];	// not necessary
	return gvc;
} //}}}

Agnode_t *gv_create_node(Agraph_t *g, int i)
{ //{{{
	Agnode_t *u;
	char buf[32];

	// label
	sprintf(buf, "%d", i);
	u = agnode(g, buf);

	// position ("!" is for fixing position)
	sprintf(buf, "%f,%f!", x[i]*8, y[i]*8);
	agsafeset(u, "pos", buf, "");

	// size		
	agsafeset(u, "fontname", "Courier", "");
	agsafeset(u, "fontsize", "8", "");
	agsafeset(u, "shape", "box", "");
	agsafeset(u, "width", "0.01", "");
	agsafeset(u, "height", "0.01", "");

	return u;
} //}}}

void gv_run(GVC_t *gvc, Agraph_t *g)
{ //{{{
	agsafeset(g, "margin", "0,0", "");
	gvLayoutJobs(gvc, g);
	gvRenderJobs(gvc, g);
	gvFreeLayout(gvc, g);
} //}}}

void gv_end(GVC_t *gvc, Agraph_t *g)
{ //{{{
	agclose(g);
	gvFreeContext(gvc);
} //}}}

double calc_P_W_X(double r)
{ //{{{
	return sqrt(3.)/4*r*r;
} //}}}

/** Uses heuristic based on effective radius. */
double calc_P_U(double r, double k)
{ //{{{
	double c = kov[int(k+1)];
	double tmp = -sqrt(c*(4-c))*(c+2) + 8*(c-1)*acos(sqrt(c)/2);
	return r*r*tmp/4/(c-4);
} //}}}

double calc_P_H(double P_Y_X, double P_U, double r)
{ //{{{
	double tmp = P_Y_X * P_U/(P_Y_X - 1);
	return (tmp -= sqrt(3.)/4/(P_Y_X - 1)*r*r);
} //}}}

int main(int argc, char **argv)
{ //{{{
	int run = 0, i, j;
	bool abort = false;

	int *deg_freq;			// stores the number of counts of a degree
	int mode_deg = 0;			// mode of degrees
	int avg_deg = 0;
	set< pair<int, int> > processed;

	vector<double> vec_jarea;	// stores joint areas of all pairs r<d<=2r apart
	vector<double> vec_P_Y_X;	// stores nY_X/nX of each sim round
	
	set< pair<int, int> > UPs;	
	vector<double> vec_uarea;	// stores joint areas of UPs
	vector<double> vec_udist;	// stores distance between u and v when they are an UP, in multiples of r (EXPERIMENT)
	
	set< pair<int, int> > HPs;
	vector<double> vec_harea;	// stores joint areas of HPs	
	

	// parse command line params
	if (argc < 3) {
		printf("Usage: %s <radio range> <number of nodes>\n", argv[0]);
		exit(1);
	}
	if (!(r = atof(argv[1]))) {
		printf("Error: Invalid radio range.\n");
		exit(1);
	}
	if (!(n = atoi(argv[2])) || n < 0) {
		printf("Error: Invalid number of nodes.\n");
		exit(1);
	}
	fprintf(stderr, "n = %d, r = %f\n", n, r);

	// parse environment variables	
	if (getenv("RIGOR")) {
		frigor = true;
		fprintf(stderr, "\tRIGOR set\n");
	} else {
		fprintf(stderr, "\tRIGOR not set\n");
	}

	if (getenv("NUM_RUNS"))	{
		NUM_RUNS = atoi(getenv("NUM_RUNS"));
	}
	fprintf(stderr, "\tNUM_RUNS = %d\n", NUM_RUNS);

	if (getenv("EUC_DIST")) {
		feuc_dist = true;
		fprintf(stderr, "\tEUC_DIST set\n");		
	} else {
		fprintf(stderr, "\tEUC_DIST not set\n");
	}

	if (getenv("NO_RES")) {
		fno_res = true;
		fprintf(stderr, "\tNO_RES set\n");		
	} else {
		fprintf(stderr, "\tNO_RES not set\n");
	}

	if (getenv("RAW")) {
		fraw = true;
		fprintf(stderr, "\tRAW set\n");		
	} else {
		fprintf(stderr, "\tRAW not set\n");
	}	

	// init vars
	x = new double[n];
	y = new double[n];
	adjlist = new set<int>[n];
	adjlist2 = new set<int>[n];
	deg_freq = new int[n];

	// start simulation rounds
	srand(123456);
	for (run = 0; run < NUM_RUNS; run++)
	{
		int nX = 0;				// incremented whenever event X occurs
		int nY_X = 0;			// incremented whenever event X and event Y occurs

		// distribute and init each node
		for (i = 0; i < n; i++) {
			x[i] = (double)rand() / RAND_MAX;
			y[i] = (double)rand() / RAND_MAX;
			for (j = 0; j < i; j++) {
				if (distance(j,i,1,1) <= r) {
					adjlist[i].insert(j);
					adjlist[j].insert(i);
				} else if (distance(j,i,1,1) <= 2*r) {
					adjlist2[i].insert(j);
					adjlist2[j].insert(i);
					vec_jarea.push_back(joint_area(j, i));
				}
			}
			deg_freq[i] = 0;
		}

		// collect statistics about degrees
		for (i = 0; i < n; i++) {
			deg_freq[adjlist[i].size()]++;
			avg_deg += adjlist[i].size();
		}
		avg_deg = int(double(avg_deg)/n);
		mode_deg = 0;
		for (i = 1; i < n; i++) {			
			if (deg_freq[i] > deg_freq[mode_deg]) {
				mode_deg = i;
			}
		}
		if (avg_deg != mode_deg && frigor) {
			printf("adg_deg(%d) != mode_deg(%d)\n", avg_deg, mode_deg);
			abort = true;
			break;
		}

		// look for an UP or an HP
		for (i = 0; i < n; i++) {
			// iterate over 2r-neighbors
			for (set<int>::const_iterator iter2 = adjlist2[i].begin(); iter2 != adjlist2[i].end(); iter2++) {
				if (pair_found_in(i, *iter2, processed))
					continue;
				
				save_pair_in(i, *iter2, processed);
				nX++;
				
				// iterate over 1-hop neighbors
				bool isUP = true;
				for (set<int>::const_iterator iter = adjlist[i].begin(); iter != adjlist[i].end(); iter++) {
					if (*iter == *iter2) continue;	// this might happen due to floating point error
					if (CONNECTED(*iter, *iter2)) {
						isUP = false;
						break;
					}
				}
				if (isUP) {
					nY_X++;
					save_pair_in(i, *iter2, UPs);
					vec_uarea.push_back(joint_area(i, *iter2));
					vec_udist.push_back(distance(i, *iter2, 1, 1)/r);
				} else {
					save_pair_in(i, *iter2, HPs);
					vec_harea.push_back(joint_area(i, *iter2));
				}
			}
		}

		vec_P_Y_X.push_back((double)nY_X/nX);

		// viz using Graphviz
		if (run == NUM_RUNS - 1) {
			GVC_t *gvc = gv_start("p.ps");
			Agraph_t *g = agopen("g", AGRAPH);
			Agnode_t **gn = new Agnode_t *[n];
			for (i = 0; i < n; i++) {
				gn[i] = NULL;
			}
			for (i = 0; i < n; i++) {
				// node
				gn[i] = gv_create_node(g, i);

				// edge
				for (set<int>::iterator iter = adjlist[i].begin(); iter != adjlist[i].end(); iter++) {
					if (*iter < i) {
						Agedge_t *e = agedge(g, gn[i], gn[*iter]);
					}
				}
			}
			gv_run(gvc, g);
			gv_end(gvc, g);
			delete[] gn;
		}

		// reset data
		for (i = 0; i < n; i++) {
			adjlist[i].clear();
			adjlist2[i].clear();
		}
		uvdist.clear();
		processed.clear();
		UPs.clear();
		HPs.clear();
	}

	if (!abort) {
		// display result
		double P_U, sim_P_W_X, sim_P_Y_X, sim_P_U, sim_P_H, stddev;

		if (!fno_res) {		
			printf("Theoretical P[W|X] : %f\n", calc_P_W_X(r));
			get_stat(vec_jarea, sim_P_W_X, stddev);
			printf("Simulated   P[W|X] : %f ��%f\n", sim_P_W_X, stddev);

			printf("Theoretical P[Y|X] : see Mathematica script\n");
			get_stat(vec_P_Y_X, sim_P_Y_X, stddev);
			printf("Simulated   P[Y|X] : %f ��%f (k = %d)\n", sim_P_Y_X, stddev, avg_deg);
			
			printf("Theoretical P_U    : %f\n", P_U = calc_P_U(r, avg_deg));
			get_stat(vec_uarea, sim_P_U, stddev);	
			printf("Simulated   P_U    : %f ��%f\n", sim_P_U, stddev);
			
			printf("Theoretical P_H    : %f\n", calc_P_H(sim_P_Y_X, P_U, r));
			get_stat(vec_harea, sim_P_H, stddev);
			printf("Simulated   P_H    : %f ��%f\n", sim_P_H, stddev);
			
			printf("P[W|X] sanity check: %f\n", sim_P_U*sim_P_Y_X + sim_P_H*(1-sim_P_Y_X)); // says sim_P_Y_X is right
		}

		// raw data
		if (fraw) {
			printf("uarea[[\"%d\"]][[\"%.2f\"]] <- c(", n, r);
			for (vector<double>::const_iterator iter = vec_uarea.begin(); iter != vec_uarea.end(); iter++) {
				if (iter + 1 == vec_uarea.end())
					printf("%f)\n", *iter);
				else
					printf("%f, ", *iter);
			}
			printf("harea[[\"%d\"]][[\"%.2f\"]] <- c(", n, r);
			for (vector<double>::const_iterator iter = vec_harea.begin(); iter != vec_harea.end(); iter++) {
				if (iter + 1 == vec_harea.end())
					printf("%f)\n", *iter);
				else
					printf("%f, ", *iter);
			}
		}
	}

	// cleanup vars
	delete[] x;
	delete[] y;
	delete[] adjlist;
	delete[] adjlist2;
	delete[] deg_freq;

	exit(abort ? 1 : 0);
} //}}}

// vim:foldmethod=marker:
