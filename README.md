# ponky-needs-ai

Needs-based utility AI implemented with a GDExtension server architecture.

## Details

This is an AI (not that kind) system for Godot Engine which provides the following types:

* Agent - a node that can choose actions based on its needs
* Action - a node that represents something an agent can do
* Need - a numerical representation of an agent's needs - e.g. hunger, thirst, repsonsibilty, socialising, etc.
* UtilityServer - a backend server that processes agent decision making on a background thread

This system was based on the famous paper by Robert Zubek (http://robert.zubek.net/publications/Needs-based-AI-draft.pdf) which describes the AI (not that kind) system used in The Sims and other games. Or, you might be familiar with Mark Brown's video about the same subject (https://www.youtube.com/watch?v=9gf2MT-IOsg) which describes the same system. This implementation is a fairly straightforward version of that system.

## How to use

Check out the repo, update the submodule and use SConstruct to build it. If I figure out how to build via github actions I'll put some better instructions.

Once loaded into Godot you should have access to the Agent and Action node types, as well as the Need resource. The UtilityServer should also be there if you want to access it directly for some reason. The documentation should describe how to use these nodes.

## Example project

There is an example project which simulates some people in an office environment. The people can be programmed with a few tasks in a queue (wait, walk to, grab/drop items, gain exclusive use of objects, loiter), but even such simple programming gives rise to fairly sophisticated behaviour. Better still, redesigning the office or adding new types of object seamlessly works with the existing code.
