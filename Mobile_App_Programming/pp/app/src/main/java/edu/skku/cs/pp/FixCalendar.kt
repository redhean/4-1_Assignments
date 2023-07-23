package edu.skku.cs.pp

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.widget.EditText
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import okhttp3.MediaType.Companion.toMediaTypeOrNull
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody
import java.io.IOException

class FixCalendar : AppCompatActivity() {

    data class Calendar(
        val name: String,
        val events: List<Event>
    )

    data class Event(
        val date: String,
        val title: String,
        val description: String,
        val check: Boolean
    )

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_fix_calendar)

        supportActionBar?.setTitle("DayFlow")

        val beforeCalendarTitle = intent.getStringExtra(MainActivity.EXT_NAME)
        val tempCalendarTitle = findViewById<EditText>(R.id.fixCalendarInput)

        val submitBtn = findViewById<Button>(R.id.submit3)

        val client = OkHttpClient()

        submitBtn.setOnClickListener {
            val newCalendarTitle = tempCalendarTitle.text.toString()

            val jsonEventData = """
                {
                    "calendar": "$beforeCalendarTitle",
                    "date": "",
                    "title": "",
                    "description": "",
                    "beforeTitle" : "$newCalendarTitle",
                    "beforeDesc" : "",
                    "check": false
                    
                }
                """.trimIndent()

            val url = "url_link"
            val requestBody = jsonEventData.toRequestBody("application/json".toMediaTypeOrNull())
            val request: Request

            request = Request.Builder().url(url).put(requestBody).build()

            client.newCall(request).enqueue(object : okhttp3.Callback {
                override fun onFailure(call: okhttp3.Call, e: IOException) {
                    e.printStackTrace()
                }

                override fun onResponse(call: okhttp3.Call, response: okhttp3.Response) {
                    if (!response.isSuccessful) throw IOException("Unexpected code $response")
                    CoroutineScope(Dispatchers.Main).launch{
                        finish()
                    }
                }
            })
        }
    }
}