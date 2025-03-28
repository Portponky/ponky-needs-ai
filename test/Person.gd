class_name Person extends CharacterBody2D

@export var full_name : String
var workstation : Node2D

enum Task {
	THINKING, # waiting for a response from utility server
	WAIT, # for a length of time
	WALK_TO, # a give destination point
	USE, # an item, which is either in hand or nearby
	GRAB, # an item which is nearby to hand
	DROP, # place item in hand on the floor
	REWARD, # grant a specific advert
}

var _current_task := Task.WAIT

var _wait_left : float

const WALK_SPEED := 50.0
var _walk_direction : Vector2


@onready var _task_queue := []

func _ready() -> void:
	# Let's add some noise to the initial values of our agents
	var noise : Dictionary[String, float] = {
		"hunger": randf_range(-0.2, 0.1),
		"thirst": randf_range(-0.1, 0.1),
		"work": randf_range(-0.2, 0.2),
		"social": randf_range(-0.1, 0.1),
		"clean": randf_range(-0.3, 0.2),
	}
	%Agent.grant(noise)
	
	# Agents are tagged with the person's name for specific actions
	# This assumes no workers have the same name, which is fine for this example
	%Agent.tags.append(full_name)
	
	# Wait for a little bit before doing something
	do_wait(1.0, 2.0)
	start_next_task()


# Populating the task queue
func do_wait(min_time: float, max_time: float) -> void:
	var time = randf_range(min_time, max_time)
	_task_queue.append([Task.WAIT, time])


func do_walk_to(pos: Vector2) -> void:
	_task_queue.append([Task.WALK_TO, pos])


func do_reward(grant: Dictionary[String, float]) -> void:
	_task_queue.append([Task.REWARD, grant])


func start_next_task() -> void:
	if _task_queue.is_empty():
		if _current_task != Task.THINKING:
			_current_task = Task.THINKING
			var tags := []
			if not workstation:
				tags.append("unassigned")
			%Agent.choose_action(200.0, 300.0, tags)
		return
	
	var task = _task_queue.pop_front()
	_current_task = task[0]
	
	match _current_task:
		Task.WAIT:
			_wait_left = task[1]
		
		Task.WALK_TO:
			_walk_direction = Vector2.ZERO
			%NavAgent.target_position = task[1]
		
		Task.REWARD:
			var reward : Dictionary[String, float] = task[1]
			%Agent.grant(reward)
			_current_task = Task.WAIT
			_wait_left = 0.1


func _on_agent_action_chosen(action: Action) -> void:
	# probe action for what to do
	var node = action.owner
	if node.has_method("plan"):
		node.plan(self, action)
	else:
		do_wait(5.0, 10.0)
	
	start_next_task()


func _on_agent_no_action_chosen() -> void:
	# Not much work doing. Wait for a bit.
	do_wait(5.0, 10.0)
	start_next_task()


func pick_random_point_goal() -> void:
	# This is a bit silly, for testing!
	var maps := NavigationServer2D.get_maps()
	%NavAgent.target_position = NavigationServer2D.map_get_random_point(maps.front(), 1, true)


func _process(delta: float) -> void:
	if _current_task == Task.WAIT:
		_wait_left -= delta
		if _wait_left < 0.0:
			start_next_task()
	elif _current_task == Task.WALK_TO:
		var offset : Vector2 = %NavAgent.get_next_path_position() - position
		var ideal := offset.normalized()
		
		if _walk_direction == Vector2.ZERO:
			_walk_direction = ideal
		
		velocity = WALK_SPEED * _walk_direction
		if move_and_slide():
			ideal = get_last_slide_collision().get_normal()
	
		var angle := _walk_direction.angle_to(ideal)
		angle = clampf(angle, -0.2, 0.2)
		_walk_direction = _walk_direction.rotated(angle)
	
		if %NavAgent.is_navigation_finished():
			start_next_task()
