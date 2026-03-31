<?php

namespace App\Http\Controllers;

use App\Models\User;
use Illuminate\Http\JsonResponse;

class UserController extends Controller
{
    /**
     * Return all users as JSON.
     *
     * @return JsonResponse
     */
    public function index(): JsonResponse
    {
        $users = User::all(['id', 'name', 'email', 'created_at']);

        return response()->json([
            'data' => $users,
            'count' => $users->count(),
        ]);
    }
}
