# Author: Matt Hejtman, login: HEJ0094
#
# Builds the EB408 classroom for exercise 8 and exports it as one OBJ.
#
# Opens assets/EB408/EB408.blend, appends the door, the whiteboard, tables and chairs
# from the LMS .blend files, puts them in place, gives the writing surface of the
# whiteboard its own texture (assets/textures/whiteboard_zpg.png) and exports
# assets/EB408/eb408.obj + eb408.mtl with the textures copied next to them.
#
# Run from the project root (Blender 4.x):
#   blender -b assets/EB408/EB408.blend --python tools/build_eb408.py
#
# Blender has Z up, OpenGL Y up: the exporter converts it (forward -Z, up Y).

import math
import os

import bmesh
import bpy
from mathutils import Vector

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
ASSETS = os.path.join(ROOT, "assets")
OUT_DIR = os.path.join(ASSETS, "EB408")

# Measured in EB408.blend by ray casts from the middle of the room (meters, Blender axes)
FRONT_WALL_X = -3.838        # -X wall (the one without windows) at y = -4
FRONT_WALL_SLOPE = 0.022     # the wall is not exactly parallel to Y: x grows by 0.022 per 1 m of y
BACK_WALL_Y = -7.49          # -Y wall with the door opening
DOOR_CENTER_X = -2.95        # door opening x = -3.45 ... -2.45, height 2.35 m
ROOM_CENTER_Y = -3.86        # side walls at y = -7.52 and -0.21
TEACHER_DESK_Y = -1.45       # 1.8 m long desk, 0.3 m from the +Y wall


def append(blend, name):
    """Appends one object from another .blend file and links it to the scene."""
    path = os.path.join(ASSETS, blend)
    with bpy.data.libraries.load(path, link=False) as (src, dst):
        dst.objects = [name]
    obj = dst.objects[0]
    bpy.context.scene.collection.objects.link(obj)
    # Image paths are relative to the source .blend ("//T_....png"), make them absolute
    for slot in obj.material_slots:
        if slot.material and slot.material.use_nodes:
            for node in slot.material.node_tree.nodes:
                if node.type == 'TEX_IMAGE' and node.image:
                    node.image.filepath = os.path.join(os.path.dirname(path), os.path.basename(node.image.filepath))
    return obj


def copy(obj, location, angle_deg):
    """Linked duplicate: shares the mesh, own position and rotation around Z."""
    dup = obj.copy()
    bpy.context.scene.collection.objects.link(dup)
    dup.location = location
    dup.rotation_euler = (0.0, 0.0, math.radians(angle_deg))
    return dup


def front_wall_x(y):
    return FRONT_WALL_X + FRONT_WALL_SLOPE * (y + 4.0)


def whiteboard_material():
    """Material with the ZPG picture as an Image Texture (map_Kd in the exported MTL)."""
    mat = bpy.data.materials.new("M_WhiteboardZPG")
    mat.use_nodes = True
    nodes = mat.node_tree.nodes
    bsdf = nodes["Principled BSDF"]
    tex = nodes.new("ShaderNodeTexImage")
    tex.image = bpy.data.images.load(os.path.join(ASSETS, "textures", "whiteboard_zpg.png"))
    mat.node_tree.links.new(tex.outputs["Color"], bsdf.inputs["Base Color"])
    return mat


def retexture_whiteboard(board):
    """Faces of the writing surface get the ZPG material and a planar UV over the whole picture."""
    mesh = board.data
    mesh.materials.append(whiteboard_material())
    index = len(mesh.materials) - 1

    bm = bmesh.new()
    bm.from_mesh(mesh)
    uv = bm.loops.layers.uv.active
    # The board faces +X (local). The writing surface = front faces facing +X,
    # the frame is around it and slightly in front of it.
    front = [f for f in bm.faces if f.normal.x > 0.99]
    front_x = max(f.calc_center_median().x for f in front)
    candidates = [f for f in front if f.calc_center_median().x < front_x - 1e-4] or front
    surface = max(candidates, key=lambda f: f.calc_area())
    surface_x = surface.calc_center_median().x
    faces = [f for f in front if abs(f.calc_center_median().x - surface_x) < 1e-4]

    ys = [v.co.y for f in faces for v in f.verts]
    zs = [v.co.z for f in faces for v in f.verts]
    y0, y1, z0, z1 = min(ys), max(ys), min(zs), max(zs)
    print(f"whiteboard surface: {len(faces)} faces, x={surface_x:.4f}, y {y0:.3f}..{y1:.3f}, z {z0:.3f}..{z1:.3f}")
    for f in faces:
        f.material_index = index
        for loop in f.loops:
            # Seen from the room (looking -X) +Y is to the right, so u grows with y
            loop[uv].uv = ((loop.vert.co.y - y0) / (y1 - y0), (loop.vert.co.z - z0) / (z1 - z0))
    bm.to_mesh(mesh)
    bm.free()


def build():
    scene = bpy.context.scene
    for obj in [o for o in scene.objects if o.type in {'CAMERA', 'LIGHT'}]:
        bpy.data.objects.remove(obj)

    # Door in the opening of the back wall. The door model has its hinge edge at x = 0
    # and is 1.04 m wide, the frame spans x = -1.10 ... 0.06.
    door = append("door/Door.blend", "SM_Door")
    frame = append("door/Door.blend", "SM_DoorFrame")
    for obj in (door, frame):
        obj.location = (DOOR_CENTER_X + 0.52, BACK_WALL_Y - 0.15, 0.0)
        obj.rotation_euler = (0.0, 0.0, 0.0)

    # Whiteboard in the middle of the front wall, turned like the wall
    board = append("Whiteboard/Whiteboard.blend", "SM_Whiteboard")
    board.location = (front_wall_x(ROOM_CENTER_Y), ROOM_CENTER_Y, 0.0)
    board.rotation_euler = (0.0, 0.0, -math.atan(FRONT_WALL_SLOPE))
    retexture_whiteboard(board)

    # Furniture (task 2b): teacher's desk in the front right corner (opposite the door,
    # next to the +Y wall), three rows of three tables with two chairs each,
    # students look at the board (-X)
    table = append("EB408Models/table.blend", "SM_Table")
    chair = append("EB408Models/OrangeChair.blend", "OrangeChair")
    table.location = (-2.7, TEACHER_DESK_Y, 0.0)
    table.rotation_euler = (0.0, 0.0, math.radians(90.0))
    copy(chair, (-3.2, TEACHER_DESK_Y, 0.0), CHAIR_ANGLE + 180.0)

    for row_x in (-1.0, 0.6, 2.2):
        for table_y in (-1.45, -3.85, -6.25):
            copy(table, (row_x, table_y, 0.0), 90.0)
            for offset in (-0.45, 0.45):
                copy(chair, (row_x + 0.55, table_y + offset, 0.0), CHAIR_ANGLE)

    bpy.data.objects.remove(chair)


# Rotation of a student chair around Z so that the sitting person looks to -X
# (the chair model faces -Y; the teacher's chair is turned by 180 degrees to face +X)
CHAIR_ANGLE = -90.0

if __name__ == "__main__":
    build()
    os.makedirs(OUT_DIR, exist_ok=True)
    bpy.ops.wm.save_as_mainfile(filepath=os.path.join(OUT_DIR, "EB408_sestaveno.blend"), copy=True)
    bpy.ops.wm.obj_export(
        filepath=os.path.join(OUT_DIR, "eb408.obj"),
        forward_axis='NEGATIVE_Z',
        up_axis='Y',
        export_uv=True,
        export_normals=True,
        export_materials=True,
        export_triangulated_mesh=True,
        path_mode='COPY',
    )
    print("exported", os.path.join(OUT_DIR, "eb408.obj"))
