extends Node2D

# A piece of paper. Allows the person to gain work value.

func _enter_tree() -> void:
	# Are we being held? Then we can't be picked up
	var held = get_parent() is Person
	%PickUp.active = !held


func plan(person: Person, action: Action) -> void:
	if action == %PickUp:
		# Person is going to pick up this piece of paper
		person.do_walk_to(self)
		person.do_grab(self)
		%PickUp.active = false
