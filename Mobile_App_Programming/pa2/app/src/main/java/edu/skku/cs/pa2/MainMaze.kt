package edu.skku.cs.pa2

import android.content.Context
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.*
import com.google.gson.Gson
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import okhttp3.*
import okhttp3.MediaType.Companion.toMediaType
import org.json.JSONObject
import java.io.IOException
import java.lang.Math.sqrt
import java.util.*

fun Context.dpToPx(dp: Int): Int {
    val density = resources.displayMetrics.density
    return (dp * density).toInt()
}


class MainMaze : AppCompatActivity() {

    data class Node(val x : Int, val y : Int)


    fun bfs(size : Int, maze: List<List<Int>>, position : Int): Array<Array<Int>>{
        val q: Queue<Node> = LinkedList()
        val visited = Array(size) { Array(size) { Int.MAX_VALUE } }

        q.offer(Node(position / size, position % size))
        visited[position / size][position % size] = 1

        while(q.isNotEmpty()){
            var left = 0
            var right = 0
            var up = 0
            var down = 0

            val target = q.poll()
            val currentDistance = visited[target.x][target.y]

            var current = maze[target.x][target.y]

            if(current >= 8){
                up = 1
                current -= 8
            }
            if(current >= 4){
                left = 1
                current -= 4
            }
            if(current >= 2){
                down = 1
                current -= 2
            }
            if(current >= 1){
                right = 1
                current -= 1
            }

            val nx = target.x
            val ny = target.y

            if (right == 0 && ny + 1 < size && visited[nx][ny + 1] > currentDistance + 1) {
                q.offer(Node(nx, ny + 1))
                visited[nx][ny + 1] = currentDistance + 1
            }

            if (down == 0 && nx + 1 < size && visited[nx + 1][ny] > currentDistance + 1) {
                q.offer(Node(nx + 1, ny))
                visited[nx + 1][ny] = currentDistance + 1
            }

            if (up == 0 && nx - 1 >= 0 && visited[nx - 1][ny] > currentDistance + 1) {
                q.offer(Node(nx - 1, ny))
                visited[nx - 1][ny] = currentDistance + 1
            }

            if (left == 0 && ny - 1 >= 0 && visited[nx][ny - 1] > currentDistance + 1) {
                q.offer(Node(nx, ny - 1))
                visited[nx][ny - 1] = currentDistance + 1
            }


            if (nx == size - 1 && ny == size - 1)
                break

        }
        return visited
    }


    class CustomAdapter(private val context: Context, private val items : List<Int>) : BaseAdapter(){
        override fun getCount(): Int {
            return items.size
        }

        override fun getItem(p0: Int): Any {
            return p0
        }

        override fun getItemId(p0: Int): Long {
            return 0
        }

        override fun getView(p0: Int, p1: View?, p2: ViewGroup?): View {
            val inflater: LayoutInflater = LayoutInflater.from(context)
            val view : View = inflater.inflate(R.layout.maze_cell,null)
            val road = view.findViewById<ImageView>(R.id.foreground)
            val icon = view.findViewById<ImageView>(R.id.icon)
            val marginInDp : Int = 3
            val marginInPx = view.context.dpToPx(marginInDp)
            var cellSizeInDp = 350 / sqrt(items.size.toDouble())
            if(cellSizeInDp % 1 != 0.0)
                cellSizeInDp = cellSizeInDp.toInt() + 1.0
            var cellSizeInPx = view.context.dpToPx(cellSizeInDp.toInt())
            val layoutParams = road.layoutParams as ViewGroup.MarginLayoutParams
            val size = sqrt(items.size.toDouble())

            //left, top, right, bottom
            var left = 0
            var top = 0
            var right = 0
            var bottom = 0

            // number >= 100 : user move top, number >= 200 : user move down, number >= 300 : user move left
            // number >= 400 : user move right, number >= 500 : hint
            var number = items[p0]

            // hint
            if(number >= 500){
                icon.setImageResource(R.drawable.hint)
                number -= 500
            }
            // right
            else if(number >= 400){
                icon.setImageResource(R.drawable.user)
                icon.rotation = 90f
                number -= 400
            }
            // left
            else if(number >= 300){
                icon.setImageResource(R.drawable.user)
                icon.rotation = 270f
                number -= 300
            }
            // down
            else if(number >= 200){
                icon.setImageResource(R.drawable.user)
                icon.rotation = 180f
                number -= 200
            }
            // top
            else if(number >= 100){
                icon.setImageResource(R.drawable.user)
                number -= 100
            }

            // goal
            if(p0 == items.size - 1){
                if(items[p0] >= 100 && items[p0] < 500)
                    icon.setImageResource(R.drawable.user)
                else if(items[p0] >= 500)
                    icon.setImageResource(R.drawable.hint)
                else
                    icon.setImageResource(R.drawable.goal)
            }

            // set margins
            if(number >= 8){
                top = marginInPx
                number -= 8
            }

            if(number >= 4){
                left = marginInPx
                number -= 4
            }

            if(number >= 2){
                bottom = marginInPx
                number -= 2
            }

            if(number >= 1) {
                right = marginInPx
                number -= 1
            }

            // Balance Maze
            if(left != 0 && right != 0)
                road.layoutParams.width = cellSizeInPx - 14
            else if(left != 0 || right != 0)
                road.layoutParams.width = cellSizeInPx - 7
            else
                road.layoutParams.width = cellSizeInPx

            if(top != 0 && bottom != 0)
                road.layoutParams.height = cellSizeInPx - 14
            else if(top != 0 || bottom != 0)
                road.layoutParams.height = cellSizeInPx - 7
            else
                road.layoutParams.height = cellSizeInPx

            if(size == 6.0 || size == 4.0){
                if(p0 / size >= size - 1)
                    road.layoutParams.height -= 7
            }


            layoutParams.setMargins(left, top, right, bottom)
            road.layoutParams = layoutParams

            return view
        }

    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main_maze)

        var hint_use = 0;
        val maze = mutableListOf<List<Int>>()
        val itemList = mutableListOf<Int>()
        var size = 0;

        // Get intent
        val maze_name = intent.getStringExtra("mazeName").toString()

        // Get request
        val client = OkHttpClient()
        val mediaType = "application/json".toMediaType()
        val gson = Gson()
        val url = "http://swui.skku.edu:1399/maze/map?name="

        val req = Request.Builder().url(url + maze_name).build()

        client.newCall(req).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                e.printStackTrace()
            }

            override fun onResponse(call: Call, response: Response) {
                if(!response.isSuccessful) throw IOException("Unexpected code $response")

                val str = response.body!!.string()
                if(str != null){
                    val jsonObject = JSONObject(str)
                    val mazeString = jsonObject.getString("maze")

                    val rows = mazeString.split("\n").filter { it.isNotBlank() }
                    size = rows.first().toInt()

                    for (i in 1 until rows.size) {
                        val row = rows[i]
                        if(row.isNotEmpty()){
                            val numbers = row.split(" ").filter { it.isNotBlank() }.map { it.toInt() }
                            maze.add(numbers)
                        }
                    }
                }

                for (row in maze) {
                    for (number in row) {
                        itemList.add(number)
                    }
                }

                itemList[0] += 100
                CoroutineScope(Dispatchers.Main).launch {
                    val mazeAdapter = CustomAdapter(this@MainMaze, itemList)
                    val main_maze = findViewById<GridView>(R.id.maze)
                    main_maze.numColumns = sqrt(itemList.size.toDouble()).toInt()
                    main_maze.adapter = mazeAdapter
                }
            }
        })

        // Adapter again
        val mazeAdapter = CustomAdapter(this@MainMaze, itemList)
        val main_maze = findViewById<GridView>(R.id.maze)
        main_maze.numColumns = sqrt(itemList.size.toDouble()).toInt()

        // Button to move
        val hint = findViewById<Button>(R.id.hint)
        val left_turn = findViewById<Button>(R.id.left)
        val right_turn = findViewById<Button>(R.id.right)
        val up_turn = findViewById<Button>(R.id.up)
        val down_turn = findViewById<Button>(R.id.down)

        // Update number of movement
        val turn = findViewById<TextView>(R.id.turn)
        var turn_cnt = 0;
        turn.text = "Trun : ${turn_cnt}"

        var position = 0;
        var hint_position = 0



        // number >= 100 : user move top, number >= 200 : user move down, number >= 300 : user move left
        // number >= 400 : user move right, number >= 500 : hint
        left_turn.setOnClickListener {
            var temp_before = itemList[position]
            itemList[position] %= 100
            var temp_after = itemList[position]
            var diff = temp_before - temp_after

            if(temp_after >= 8)
                temp_after -= 8
            if(temp_after >= 4){
                itemList[position] += 300
                main_maze.adapter = mazeAdapter
            }
            else{
                turn_cnt++
                turn.text = "Trun : ${turn_cnt}"
                position--
                itemList[position] += 300

                if(position == hint_position && itemList[hint_position] >= 500)
                    itemList[hint_position] -= 500

                main_maze.adapter = mazeAdapter

                if(position == main_maze.numColumns * main_maze.numColumns - 1)
                    Toast.makeText(this,"finish!",Toast.LENGTH_SHORT).show()
            }

        }

        right_turn.setOnClickListener {
            var temp_before = itemList[position]
            itemList[position] %= 100
            var temp_after = itemList[position]
            var diff = temp_before - temp_after

            if(temp_after >= 8)
                temp_after -= 8
            if(temp_after >= 4)
                temp_after -= 4
            if(temp_after >= 2)
                temp_after -= 2
            if(temp_after >= 1){
                itemList[position] += 400
                main_maze.adapter = mazeAdapter
            }
            else{
                turn_cnt++
                turn.text = "Trun : ${turn_cnt}"
                position++
                itemList[position] += 400
                if(position == hint_position && itemList[hint_position] >= 500)
                    itemList[hint_position] -= 500

                main_maze.adapter = mazeAdapter

                if(position == main_maze.numColumns * main_maze.numColumns - 1)
                    Toast.makeText(this,"finish!",Toast.LENGTH_SHORT).show()
            }
        }

        up_turn.setOnClickListener {
            var temp_before = itemList[position]
            itemList[position] %= 100
            var temp_after = itemList[position]
            var diff = temp_before - temp_after

            if(temp_after >= 8){
                itemList[position] += 100
                main_maze.adapter = mazeAdapter
            }
            else{
                turn_cnt++
                turn.text = "Trun : ${turn_cnt}"
                position -= main_maze.numColumns
                itemList[position] += 100
                if(position == hint_position && itemList[hint_position] >= 500)
                    itemList[hint_position] -= 500

                main_maze.adapter = mazeAdapter

                if(position == main_maze.numColumns * main_maze.numColumns - 1)
                    Toast.makeText(this,"finish!",Toast.LENGTH_SHORT).show()
            }
        }

        down_turn.setOnClickListener {
            var temp_before = itemList[position]
            itemList[position] %= 100
            var temp_after = itemList[position]
            var diff = temp_before - temp_after

            if(temp_after >= 8)
                temp_after -= 8
            if(temp_after >= 4)
                temp_after -= 4
            if(temp_after >= 2){
                itemList[position] += 200
                main_maze.adapter = mazeAdapter
            }
            else{
                turn_cnt++
                turn.text = "Trun : ${turn_cnt}"
                position += main_maze.numColumns
                itemList[position] += 200
                if(position == hint_position && itemList[hint_position] >= 500)
                    itemList[hint_position] -= 500

                main_maze.adapter = mazeAdapter

                if(position == main_maze.numColumns * main_maze.numColumns - 1)
                    Toast.makeText(this,"finish!",Toast.LENGTH_SHORT).show()
            }
        }

        hint.setOnClickListener {
            // BFS
            val size_bfs = main_maze.numColumns
            val visited = bfs(size_bfs, maze, position)

            var shortest = visited[size_bfs - 1][size_bfs - 1] - 1
            val path = MutableList(shortest + 2) { 0 }
            val temp_path = mutableListOf<Int>()

            for (row in visited) {
                for (number in row) {
                    temp_path.add(number)
                }
            }

            val start = shortest
            path[start + 1] = size_bfs * size_bfs - 1

            while(shortest >= 0){
                for(i in temp_path.size - 1 downTo  0){
                    if(temp_path[i] == shortest) {

                        //left
                        if(i + 1 < temp_path.size && temp_path[i + 1] == shortest + 1){
                            var wall_chk  = itemList[i + 1]
                            if(wall_chk >= 8)
                                wall_chk -= 8
                            if(wall_chk >= 4){
                                continue
                            }
                            else {
                                path[shortest] = i
                                break
                            }
                        }
                        //right
                        if(i - 1 >= 0 && temp_path[i - 1] == shortest + 1){
                            var wall_chk  = itemList[i - 1]
                            if(wall_chk >= 8)
                                wall_chk -= 8
                            if(wall_chk >= 4)
                                wall_chk -= 4
                            if(wall_chk >= 2)
                                wall_chk -= 2
                            if(wall_chk >= 1){
                                continue
                            }
                            else {
                                path[shortest] = i
                                break
                            }
                        }
                        //up
                        if(i - size_bfs >= 0 && temp_path[i - size_bfs] == shortest + 1){
                            var wall_chk  = itemList[i - size_bfs]
                            if(wall_chk >= 8)
                                wall_chk -= 8
                            if(wall_chk >= 4)
                                wall_chk -= 4
                            if(wall_chk >= 2){
                                continue
                            }
                            else {
                                path[shortest] = i
                                break
                            }
                        }
                        //down
                        if(i + size_bfs < temp_path.size && temp_path[i + size_bfs] == shortest + 1){
                            var wall_chk  = itemList[i + size_bfs]
                            if(wall_chk >= 8){
                                continue
                            }
                            else {
                                path[shortest] = i
                                break
                            }
                        }
                    }
                }
                shortest--
            }

            for(i in 0 until path.size){
                print(path[i].toString())
                print(" ")
            }

            // Set hint
            for(i in 1 until start + 2){
                if(path[i]  == position){
                    if(i == start + 1)
                        hint_position = path[i]
                    else
                        hint_position = path[i + 1]
                    break
                }
            }

            if(hint_use == 0) {
                itemList[hint_position] += 500
                main_maze.adapter = mazeAdapter
            }

            // hint can use only once
            hint_use = 1
        }

    }
}