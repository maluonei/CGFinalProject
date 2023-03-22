#pragma once
#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include "vec2.h"
#include <vector>
#include <algorithm>

bool linearT = false;

void ConquerT(vector<point2>& leftConvex, vector<point2>& rightConvex, vector<point2>& finalConvex, vector<pair<point2, point2>>& segs) {
	int leftConvexRightest = 0;
	int rightConvexLeftest = 0;
	int leftSize = leftConvex.size();
	int rightSize = rightConvex.size();
	while (true) {
		int before = (rightConvexLeftest - 1 + rightSize) % rightSize;
		int after = (rightConvexLeftest + 1) % rightSize;
		if (rightConvex[rightConvexLeftest].x <= rightConvex[before].x && rightConvex[rightConvexLeftest].x <= rightConvex[after].x) {
			break;
		}
		rightConvexLeftest++;
	}

	int leftUp = leftConvexRightest, leftDown = leftConvexRightest, rightUp = rightConvexLeftest, rightDown = rightConvexLeftest;
	//zigzagUp
	while (true) {
		bool leftFlag = false, rightFlag = false;
		int rightNext = (rightUp - 1 + rightSize) % rightSize;
		int leftNext = (leftUp + 1) % leftSize;
		while (true) {
			rightNext = (rightUp - 1 + rightSize) % rightSize;
			if (InLeftTest(leftConvex[leftUp], rightConvex[rightUp], rightConvex[rightNext])) {
				segs.push_back(make_pair(leftConvex[leftUp], rightConvex[rightUp]));
				rightFlag = true;
				rightUp = rightNext;
			}
			else {
				break;
			}
		}
		while (true) {
			leftNext = (leftUp + 1) % leftSize;
			if (!InLeftTest_noEqual(rightConvex[rightUp], leftConvex[leftUp], leftConvex[leftNext])) {
				segs.push_back(make_pair(leftConvex[leftUp], rightConvex[rightUp]));
				leftFlag = true;
				leftUp = leftNext;
			}
			else {
				break;
			}
		}
		if (!leftFlag && !rightFlag) break;
	}
	segs.push_back(make_pair(leftConvex[leftUp], rightConvex[rightUp]));

	//zigzagDown
	while (true) {
		bool leftFlag = false, rightFlag = false;
		int rightNext = (rightDown + 1) % rightSize;
		int leftNext = (leftDown - 1 + leftSize) % leftSize;
		while (true) {
			rightNext = (rightDown + 1) % rightSize;
			if (!InLeftTest_noEqual(leftConvex[leftDown], rightConvex[rightDown], rightConvex[rightNext])) {
				segs.push_back(make_pair(leftConvex[leftDown], rightConvex[rightDown]));
				rightFlag = true;
				rightDown = rightNext;
			}
			else {
				break;
			}
		}
		while (true) {
			leftNext = (leftDown - 1 + leftSize) % leftSize;
			if (InLeftTest(rightConvex[rightDown], leftConvex[leftDown], leftConvex[leftNext])) {
				segs.push_back(make_pair(leftConvex[leftDown], rightConvex[rightDown]));
				leftFlag = true;
				leftDown = leftNext;
			}
			else {
				break;
			}
		}
		if (!leftFlag && !rightFlag) break;
	}
	segs.push_back(make_pair(leftConvex[leftDown], rightConvex[rightDown]));

	if (leftDown == 0) leftDown = leftSize;
	if (rightDown == 0) rightDown = rightSize;
	//construct finalConvex
	for (int i = 0; i <= rightUp; i++) {
		finalConvex.push_back(rightConvex[i]);
	}
	for (int i = leftUp; i <= leftDown; i++) {
		finalConvex.push_back(leftConvex[i % leftSize]);
	}
	for (int i = rightDown; i < rightSize; i++) {
		finalConvex.push_back(rightConvex[i]);
	}

	for (int i = rightUp; i < rightDown; i++) {
		segs.push_back(make_pair(rightConvex[i % rightSize], rightConvex[(i + 1) % rightSize]));
	}
	for (int i = leftDown; i < leftUp + leftSize; i++) {
		segs.push_back(make_pair(leftConvex[i % leftSize], leftConvex[(i + 1) % leftSize]));
	}

	//cout << "leftUp:" << leftUp << "  " << leftConvex[leftUp] << endl;
	//cout << "leftDown:" << leftDown << "  " << leftConvex[leftDown % leftSize] << endl;
	//cout << "rightUp:" << rightUp << "  " << rightConvex[rightUp] << endl;
	//cout << "rightUDown:" << rightDown << "  " << rightConvex[rightDown % rightSize] << endl;
	//for (int i = 0; i < finalConvex.size(); i++) {
	//	cout << finalConvex[i] << endl;
	//}
}

void DevideAndConquerT(vector<point2>& points, vector<point2>& convexHull, vector<pair<point2, point2>>& segs, int start, int end) {
	int mid = (start + end) / 2;
	int pointNum = end - start + 1;

	if (points[start].x == 8078 || points[end].x == 8078) {
		int i = 0;
	}

	if (pointNum == 1) {
		convexHull.push_back(points[end]);
	}
	else if (pointNum == 2) {
		convexHull.push_back(points[end]);
		convexHull.push_back(points[start]);
	}
	else if (pointNum == 3) {
		convexHull.push_back(points[end]);
		if (InLeftTest(points[end], points[start], points[mid])) {
			convexHull.push_back(points[start]);
			convexHull.push_back(points[mid]);
		}
		else {
			convexHull.push_back(points[mid]);
			convexHull.push_back(points[start]);
		}
	}
	else if (pointNum > 3) {
		if (points[start].x == points[end].x) {
			if (linear) {
				for (int j = end; j >= start; j--) {
					convexHull.push_back(points[j]);
				}
			}
			else {
				convexHull.push_back(points[end]);
				convexHull.push_back(points[start]);
			}
			return;
		}

		vector<point2> leftConvex, rightConvex;

		if (points[start].x == points[mid].x && mid - start >= 2) {
			while (points[start].x == points[mid].x) {
				mid++;
			}
			if (mid + 1 == end) {
				if (linearT) {
					for (int i = mid - 1; i >= start; i--) {
						leftConvex.push_back(points[i]);
					}
				}
				else {  //start, mid-1, mid, end -->  start---mid-1的x坐标相同，且右边只剩mid，end
					leftConvex.push_back(points[mid - 1]);
					leftConvex.push_back(points[start]);
				}
				rightConvex.push_back(points[end]);
				rightConvex.push_back(points[mid]);
				ConquerT(leftConvex, rightConvex, convexHull, segs);
				return;
			}
			if (mid == end) {
				if (linearT) {
					for (int j = end; j >= start; j--) {
						convexHull.push_back(points[j]);
					}
				}
				else {
					convexHull.push_back(points[end]);
					convexHull.push_back(points[end - 1]);
					convexHull.push_back(points[start]);
				}
				return;
			}

			if (linearT) {
				for (int j = mid; j >= start; j--) {
					leftConvex.push_back(points[j]);
				}
			}
			else {
				leftConvex.push_back(points[mid]);
				leftConvex.push_back(points[mid - 1]);
				leftConvex.push_back(points[start]);
			}
			DevideAndConquerT(points, rightConvex, segs, mid + 1, end);
		}
		else if (points[mid + 1].x == points[end].x && end - mid >= 3) {
			while (points[mid + 1].x == points[end].x) {
				mid--;
			}
			if (mid == start) {
				leftConvex.push_back(points[mid]);
				leftConvex.push_back(points[start]);
				rightConvex.push_back(points[end]);
				if (linear) {
					for (int i = mid + 1; i < end; i++) {
						rightConvex.push_back(points[i]);
					}
				}
				else { //start, mid, mid+1, end ，mid+1......end的x坐标相同，且左边只剩start，mid
					rightConvex.push_back(points[mid + 1]);
				}
				ConquerT(leftConvex, rightConvex, convexHull, segs);
				return;
			}
			if (mid + 1 == start) {
				if (linearT) {
					convexHull.push_back(points[end]);
					for (int j = start; j < end; j++) {
						convexHull.push_back(points[j]);
					}
				}
				else {
					convexHull.push_back(points[end]);
					convexHull.push_back(points[start]);
					convexHull.push_back(points[start + 1]);
				}

				return;
			}

			if (linearT) {
				rightConvex.push_back(points[end]);
				for (int j = mid + 1; j < end; j++) {
					rightConvex.push_back(points[j]);
				}
			}
			else {
				rightConvex.push_back(points[end]);
				rightConvex.push_back(points[mid + 1]);
				rightConvex.push_back(points[mid + 2]);
			}

			DevideAndConquerT(points, leftConvex, segs, start, mid);
		}
		else {
			DevideAndConquerT(points, leftConvex, segs, start, mid);
			DevideAndConquerT(points, rightConvex, segs, mid + 1, end);
		}
		ConquerT(leftConvex, rightConvex, convexHull, segs);
	}
}

//Verified
vector<pair<point2, point2>> zigzagTriangulation(vector<point2>& points) {
	vector<point2> convexHull;
	vector<pair<point2, point2>> segs;
	int start = 0;
	int end = points.size() - 1;
	sort(points.begin(), points.end(), ComparatorXmin);
	DevideAndConquerT(points, convexHull, segs, start, end);

	int length = convexHull.size();
	for (int i = 0; i < length; i++) {
		segs.push_back(make_pair(convexHull[i], convexHull[(i + 1) % length]));
	}

	return segs;
}
#endif

