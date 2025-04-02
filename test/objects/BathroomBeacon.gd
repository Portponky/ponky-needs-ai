extends Node2D


func _ready() -> void:
	$Sprite.visible = false


func plan(person: Person, action: Action) -> void:
	if action == %Beacon:
		person.do_walk_to(global_position)
		person.do_loiter(0.2)
