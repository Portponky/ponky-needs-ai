extends Node2D


func _ready() -> void:
	$Sprite.visible = false


func plan(person: Person, action: Action) -> void:
	if action == %Beacon:
		person.do_drop() # don't carry food or work into the bathroom
		person.do_walk_to(self)
		person.do_loiter(0.2)
