#ifndef ACTION_H
#define ACTION_H

#include "scene.h"
#include <queue>

class action : public std::enable_shared_from_this<action> {
public:
	using callback_func = std::function<void(std::shared_ptr<action>,std::shared_ptr<action>)>;

	action() : n(), s_frame(0), remaining_action(false) {}
	action(std::shared_ptr<node> n_, int start_frame_, callback_func did_finish_ = nullptr) :
			n(n_), s_frame(start_frame_), did_finish(did_finish_), remaining_action(false) {}
	// void start_next_action_at(int frame_number);
	
	virtual bool act(int frame_number); // returns true if action is still active - it hasn't yet finished.
	virtual bool own_act() {return false;} // perform this node's action, as opposed the actions of its children
	virtual int get_start_frame() {return s_frame;}
	virtual void set_start_frame(int frame_number) {s_frame = frame_number;}
	
	int get_current_frame() {return current_frame;}
	std::shared_ptr<node> get_node() {return n;}
	bool has_started(int frame_number) {return get_start_frame() <= frame_number;}
	void add_child(std::shared_ptr<action> child) {children.push_back(child);}

	virtual void will_start() {} // called by the act() method when get_start_frame() is the current frame
	// returns a pointer to a function to be called by this's parent
	// when this action finishes
	// signature:  void did_finish(std::shared_ptr<action> parent, std::shared_ptr<action> finished_action)
	callback_func did_finish;

protected:
	std::shared_ptr<node> n;
	std::vector<std::shared_ptr<action>> children;
	int s_frame;  // starting frame number in the larger animation
	int current_frame = 0;  // frames since the start of the action
	bool remaining_action;
};

class move : public action {
	friend class move;
public:
	/*
	fixed: 	frame count for an action is a constant value
	speed: 	a node moves a constant node_speed during an action;
			the number of frames varies depending on distance
			the node travels during the action.
	limit: 	nodes prefer to move at node_speed during an action,
			but the number of frames in an action is limited
			by low_frame_limit and high_frame_limit.
			The speed of a node may adjust to ensure the action
			takes no more or fewer frames than the specified limits.
	Actions defined in derived classes should implement these semantics.
	*/
	enum frame_count_method_type : int {fixed = 0, speed = 1, limit = 2};
	
	move(std::shared_ptr<node> n_, int start_frame_, frame_count_method_type fcm = fixed) : 
		action(n_,start_frame_),  frame_count_method(fcm) {}

	void set_fixed_frame_count(int ffc) {fixed_frame_count = ffc; global_update();}
	void set_node_speed(int ns) {node_speed = ns; global_update();}
	void set_low_frame_limit(int lfl) {low_frame_limit = lfl; global_update();}
	void set_high_frame_limit(int hfl) {high_frame_limit = hfl; global_update();}
	int get_fixed_frame_count() {return fixed_frame_count;}
	int get_node_speed() {return node_speed;}
	int get_high_frame_limit() {return high_frame_limit;}
	int get_low_frame_limit() {return low_frame_limit;}


	virtual void use_frame_count() {use_frame_count_method(fixed);} 
	virtual void use_node_speed() {use_frame_count_method(speed);}
	virtual void use_frame_limit() {use_frame_count_method(limit);}
	virtual void use_frame_count_method(frame_count_method_type fcm) {frame_count_method = fcm; global_update();}

#ifndef PRIVACY_OFF
protected:	
#endif
	int fixed_frame_count = 1; // number of frames an action spans
	int node_speed = 1; // speed at which a node should change location during an action, in scene coordinates per frame
	int high_frame_limit = 1000; // greatest number of frames an action should take
	int low_frame_limit = 0; // least number of frames an action should take

	frame_count_method_type frame_count_method;
	virtual void internal_update() {}
	virtual void global_update(); // update this object's internal settings and those of its children

	virtual void update_children_frame_count_settings();
};

class shift : public move {
public:
	shift(std::shared_ptr<node> n_, int start_frame_, int dx_, int dy_, frame_count_method_type fcm = fixed);	
	virtual bool own_act() override;
	int get_f_count() {return f_count;}

private:
	virtual void internal_update() override {compute_f_count_and_increments();}
	void compute_f_count();
	void compute_increments();
	void compute_f_count_and_increments();
	int f_count;
	int x_increment;
	int x_r;
	int y_increment;
	int y_r;
	int dx;
	int dy;
};

class space_children : public move {
public:
	space_children(std::shared_ptr<node> n_, int start_frame_, int dx_, int dy_, frame_count_method_type fcm = fixed);
	virtual bool own_act() override;
	virtual void update_children_frame_count_settings() override;

private:
	int dx;
	int dy;
	void create_children();
};

class settle : public move {
public:
	settle(std::shared_ptr<node> n_, int start_frame_, dimension dim_ = dimension::y,
			 int target_coordinate_ = 0,frame_count_method_type fcm = fixed);
	
#ifndef PRIVACY_OFF
private:
#endif
	dimension dim = dimension::y;
	int target_coordinate = 0;
	void create_children();
};

#endif
