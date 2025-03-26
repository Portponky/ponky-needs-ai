extends CharacterBody2D

@export var full_name : String

const WALK_SPEED := 50.0
var walk_direction : Vector2

func _ready() -> void:
	# Navigation isn't ready immediately
	await get_tree().process_frame
	pick_random_point_goal()


func think() -> void:
	$Agent.choose_action(200.0, 300.0)


func _on_agent_action_chosen(action: Action) -> void:
	$Agent.grant(action.advert)
	print($Agent.get_need_value("happy"))


func _on_agent_no_action_chosen() -> void:
	print("No action")


func pick_random_point_goal() -> void:
	# This is a bit silly, doesn't even check if it's a valid point
	var maps := NavigationServer2D.get_maps()
	%NavAgent.target_position = NavigationServer2D.map_get_random_point(maps.front(), 1, true)


func _process(delta: float) -> void:
	var offset : Vector2 = %NavAgent.get_next_path_position() - position
	var ideal := offset.normalized()
	
	walk_direction = (1.1 * walk_direction + ideal).normalized()
	
	velocity = WALK_SPEED * walk_direction
	if move_and_slide():
		walk_direction = get_last_slide_collision().get_normal()
	
	if %NavAgent.is_navigation_finished():
		pick_random_point_goal()
