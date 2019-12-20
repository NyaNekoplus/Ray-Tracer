#ifndef BVH_NODE_H_
#define BVH_NODE_H_

#include "hitable.h"

int box_x_compare(const void* a, const void* b) {
	aabb box_left, box_right;
	hitable* ah = *(hitable**)a;
	hitable* bh = *(hitable**)b;
	if(!ah->bounding_box(0,0,box_left)||!bh->bounding_box(0,0,box_right))
		std::cerr << "no bounding box in bvh_node constructor\n";
	if (box_left.min().x() - box_right.min().x() < 0.0)				//快排比较函数
		return -1;
	else
		return 1;
}

int box_y_compare(const void* a, const void* b) {
	aabb box_left, box_right;
	hitable* ah = *(hitable * *)a;
	hitable* bh = *(hitable * *)b;
	if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
		std::cerr << "no bounding box in bvh_node constructor\n";
	if (box_left.min().y() - box_right.min().y() < 0.0)				//快排比较函数
		return -1;
	else
		return 1;
}

int box_z_compare(const void* a, const void* b) {
	aabb box_left, box_right;
	hitable* ah = *(hitable * *)a;
	hitable* bh = *(hitable * *)b;
	if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
		std::cerr << "no bounding box in bvh_node constructor\n";
	if (box_left.min().z() - box_right.min().z() < 0.0)				//快排比较函数
		return -1;
	else
		return 1;
}

class bvh_node :public hitable {
public:
	bvh_node() {}
	bvh_node(hitable** l, int n, double time0, double time1);
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec)const;
	virtual bool bounding_box(double t0, double t1, aabb& b)const;	//time0,time1

	hitable* left;
	hitable* right;
	aabb box;
};

bvh_node::bvh_node(hitable** l, int n, double time0, double time1) {
	int axis = int(3 * randow1);
	if (axis == 0)
		qsort(l, n, sizeof(hitable*), box_x_compare);
	else if(axis==1)
		qsort(l, n, sizeof(hitable*), box_y_compare);
	else
		qsort(l, n, sizeof(hitable*), box_z_compare);
	if (n == 1) {
		left = right = l[0];
	}
	else if (n == 2) {
		left = l[0];
		right = l[1];
	}
	else {
		left = new bvh_node(l, n / 2, time0, time1);
		right = new bvh_node(l + n / 2, n - n / 2, time0, time1);
	}
	aabb box_left, box_right;
	if (!left->bounding_box(time0, time1, box_left) || !right->bounding_box(time0, time1, box_right)) {
		std::cerr << "no bounding box in bvh_node constructor\n";	//二者都为真时不输出错误。即只要有一个节点不能
	}																//创建包围盒，则输出错误
	box = surrounding_box(box_left, box_right);		//左右节点的总包围盒
}

inline bool bvh_node::hit(const ray& r, double t_min, double t_max, hit_record& rec) const{
	if (box.hit(r, t_min, t_max)) {
		hit_record left_rec, right_rec;
		bool hit_left = left->hit(r, t_min, t_max, left_rec);		//记录bvh树左端是否有碰撞
		bool hit_right = right->hit(r, t_min, t_max, right_rec);	//记录bvh树右端是否有碰撞
		if (hit_left && hit_right) {
			if (left_rec.t < right_rec.t)			//光线长度比较
				rec = left_rec;
			else
				rec = right_rec;
			return true;
		}
		else if (hit_left) {
			rec = left_rec;
			return true;
		}
		else if (hit_right) {
			rec = right_rec;
			return true;
		}
		else return false;
	}
	else return false;
}

bool bvh_node::bounding_box(double t0, double t1, aabb& b)const {
	b = box;
	return true;
}

#endif